// Copyright 2018-2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <thread>
#include <boost/program_options.hpp>
#include "sysexits.h"

#include "../utilities/NoInputException.h"
#include "../utilities/UsageException.h"
#include "../utilities/DataFormatException.h"
#include "../utilities/NoOutputException.h"
#include "SelectionCriterion.h"
#include "DenseRoughGraphTensor.h"
#include "DenseRoughTensor.h"
#include "PatternFileReader.h"
#include "ConcurrentPatternPool.h"
#include "ModifiedGraphPattern.h"
#include "RankPatterns.h"

using namespace boost::program_options;

int main(int argc, char* argv[])
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  AbstractRoughTensor* roughTensor;
  float verboseStep = 0;
  int maxSelectionSize = 0;
  SelectionCriterion selectionCriterion;
  bool isRSSPrinted;
  {
#ifdef DETAILED_TIME
    steady_clock::time_point startingPoint;
#endif
    vector<thread> threads;
    bool isModifyingOrGrowing;
    {
      PatternFileReader patternFileReader;
      unsigned int secondVertexDimension;
      long long maxNbOfInitialPatterns = 0;
      {
	int nbOfJobs;
	bool isGrow;
	// Parsing the command line and the option file
	try
	  {
	    string tensorFileName;
	    string outputFileName;
	    options_description generic("Generic options");
	    generic.add_options()
	      ("help,h", "produce this help message")
	      ("hio", "produce help on Input/Output format")
	      ("version,V", "display version information and exit")
	      ("opt", value<string>(), "set the option file name (by default, [tensor-file].opt if present)");
	    options_description basicConfig("Basic configuration (on the command line or in the option file)");
	    basicConfig.add_options()
	      ("verbose,v", value<float>(), "verbose output every arg seconds")
	      ("boolean,b", "consider the tensor Boolean")
	      ("communities,c", value<string>(), "set vertex dimensions ids (1 is the first)")
	      ("out,o", value<string>(&outputFileName)->default_value("-"), "set output file name")
	      ("max,m", value<long long>(), "bound nb of initial patterns with arg")
	      ("remember,r", value<float>()->default_value(0), "if possible, use up to arg GB of RAM and be faster")
#ifdef DEBUG_MODIFY
	      ("jobs,j", value<int>(&nbOfJobs)->default_value(1), "set nb of simultaneous searches of patterns")
#else
	      ("jobs,j", value<int>(&nbOfJobs)->default_value(max(thread::hardware_concurrency(), static_cast<unsigned int>(1))), "set nb of simultaneous searches of patterns")
#endif
	      ("density,d", value<float>(), "set threshold between 0 (dense storage of the data) and 1 (default, minimal memory usage)")
	      ("msc", value<string>()->default_value("bic"), "set max selection criterion (rss, aic or bic)")
	      ("mss", value<int>(), "set max selection size (by default, unbounded)")
	      ("ns", "neither select nor rank output patterns")
	      ("shift,s", value<float>(), "shift memberhip degrees by constant in argument (by default, density of input tensor)")
	      ("expectation,e", "shift every memberhip degree by the max density of the slices covering it")
	      ("patterns,p", value<string>(), "set initial patterns, instead of the default ones")
	      ("os", value<string>(), "only select and rank the provided patterns")
	      ("grow,g", "remove nothing from the initial patterns");
	    options_description io("Input/Output format (on the command line or in the option file)");
	    io.add_options()
	      ("tds", value<string>()->default_value(" "), "set any character separating dimensions in input tensor")
	      ("tes", value<string>()->default_value(","), "set any character separating elements in input tensor")
	      ("pds", value<string>()->default_value(" "), "set any character separating dimensions in input patterns")
	      ("pes", value<string>()->default_value(","), "set any character separating elements in input patterns")
	      ("ods", value<string>()->default_value(" "), "set string separating dimensions in output patterns")
	      ("oes", value<string>()->default_value(","), "set string separating elements in output patterns")
	      ("pl", "print densities in the shifted (rather than input) tensor in output")
	      ("ps", "print sizes in output")
	      ("sp", value<string>()->default_value(" : "), "set string prefixing sizes in output")
	      ("ss", value<string>()->default_value(" "), "set string separating sizes in output")
	      ("pa", "print areas in output")
	      ("ap", value<string>()->default_value(" : "), "set string prefixing area in output")
	      ("pr", "print residual sum of squares of truncated output")
	      ("rp", value<string>()->default_value(" : "), "set string prefixing residual sum of squares in output");
	    options_description hidden("Hidden options");
	    hidden.add_options()
	      ("file", value<string>(&tensorFileName)->default_value("-"), "set tensor file");
	    positional_options_description p;
	    p.add("file", -1);
	    options_description commandLineOptions;
	    commandLineOptions.add(generic).add(basicConfig).add(io).add(hidden);
	    variables_map vm;
	    store(command_line_parser(argc, argv).options(commandLineOptions).positional(p).run(), vm);
	    notify(vm);
	    if (vm.count("help"))
	      {
		cout << "Usage: nclusterbox [options] [tensor-file]\n" << generic << basicConfig << "\nReport bugs to: lcerf@dcc.ufmg.br\n";
		return EX_OK;
	      }
	    if (vm.count("hio"))
	      {
		cout << io;
		return EX_OK;
	      }
	    if (vm.count("version"))
	      {
		cout << "nclusterbox version 0.61\nCopyright (C) 2023-2026 Loïc Cerf.\nLicense GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n";
		return EX_OK;
	      }
	    if (vm.count("opt"))
	      {
		options_description config;
		config.add(basicConfig).add(io).add(hidden);
		if (vm["opt"].as<string>() == "-")
		  {
		    store(parse_config_file(cin, config), vm);
		  }
		else
		  {
		    ifstream optionFile(vm["opt"].as<string>());
		    if (!optionFile)
		      {
			throw NoInputException(vm["opt"].as<string>().c_str());
		      }
		    store(parse_config_file(optionFile, config), vm);
		  }
		notify(vm);
	      }
	    else
	      {
		if (vm.count("file"))
		  {
		    ifstream optionFile((tensorFileName + ".opt").c_str());
		    options_description config;
		    config.add(basicConfig).add(io).add(hidden);
		    store(parse_config_file(optionFile, config), vm);
		    optionFile.close();
		    notify(vm);
		  }
	      }
	    if (vm.count("max"))
	      {
		if (vm["max"].as<long long>() < 1)
		  {
		    throw UsageException("max option should provide a positive integer!");
		  }
		maxNbOfInitialPatterns = vm["max"].as<long long>();
	      }
	    unsigned int firstVertexDimension;
	    if (vm.count("communities"))
	      {
		istringstream ss(vm["communities"].as<string>());
		if (!(ss >> firstVertexDimension))
		  {
		    secondVertexDimension = 0;
		  }
		else
		  {
		    if (ss >> secondVertexDimension)
		      {
			{
			  int thirdVertexDimension;
			  if (ss >> thirdVertexDimension || firstVertexDimension == secondVertexDimension)
			    {
			      throw UsageException("communities option should provide one or two different dimension ids (1 is the first)!");
			    }
			}
			if (--firstVertexDimension > --secondVertexDimension)
			  {
			    swap(firstVertexDimension, secondVertexDimension);
			  }
		      }
		    else
		      {
			if (!firstVertexDimension)
			  {
			    throw UsageException("communities option should provide one or two different dimension ids (1 is the first)!");
			  }
			secondVertexDimension = firstVertexDimension--;
		      }
		  }
	      }
	    else
	      {
		secondVertexDimension = 0;
	      }

	    if (vm.count("ns"))
	      {
		if (vm.count("verbose") && outputFileName == "-")
		  {
		    throw UsageException("ns and verbose together forbid to output to the standard output!");
		  }
		for (const char* const& ignoredOption : {"mss", "pr"})
		  {
		    if (vm.count(ignoredOption))
		      {
			cerr << "Warning: " << ignoredOption << " option ignored, because ns option used\n";
		      }
		  }
	      }
	    if (vm.count("verbose"))
	      {
		verboseStep = vm["verbose"].as<float>();
		if (!verboseStep)
		  {
		    verboseStep = -1;
		  }
	      }
	    const string sc = vm["msc"].as<string>();
	    if (sc == string("rss").substr(0, sc.size()))
	      {
		selectionCriterion = rss;
	      }
	    else
	      {
		if (sc == string("aic").substr(0, sc.size()))
		  {
		    selectionCriterion = aic;
		  }
		else
		  {
		    if (sc == string("bic").substr(0, sc.size()))
		      {
			selectionCriterion = bic;
		      }
		    else
		      {
			throw UsageException("msc option should provide either \"rss\" or \"aic\" or \"bic\"!");
		      }
		  }
	      }
	    if (vm.count("mss"))
	      {
		if (vm["mss"].as<int>() < 1)
		  {
		    throw UsageException("mss option should provide a positive integer!");
		  }
		maxSelectionSize = vm["mss"].as<int>();
	      }
	    float density;
	    if (vm.count("density"))
	      {
		density = vm["density"].as<float>();
		if (density < 0 || density > 1)
		  {
		    throw UsageException("density option should provide a float in [0, 1]!");
		  }
	      }
	    else
	      {
		density = 1;
	      }
	    if (vm.count("os"))
	      {
		for (const char* const& ignoredOption : {"ns", "patterns", "grow"})
		  {
		    if (vm.count(ignoredOption))
		      {
			cerr << "Warning: " << ignoredOption << " option ignored, because os option used\n";
		      }
		  }
		patternFileReader.openFile(vm["os"].as<string>().c_str(), vm["pds"].as<string>().c_str(), vm["pes"].as<string>().c_str());
		isModifyingOrGrowing = false;
	      }
	    else
	      {
		if (vm["remember"].as<float>() < 0)
		  {
		    throw UsageException("remember option should provide a positive float!");
		  }
		if (vm["jobs"].as<int>() < 1)
		  {
		    throw UsageException("jobs option should provide a positive integer!");
		  }
		if (vm.count("patterns"))
		  {
		    patternFileReader.openFile(vm["patterns"].as<string>().c_str(), vm["pds"].as<string>().c_str(), vm["pes"].as<string>().c_str());
		  }
		else
		  {
		    ConcurrentPatternPool::setDefaultPatterns(maxNbOfInitialPatterns);
		  }
		isModifyingOrGrowing = true;
	      }
	    if (vm.count("expectation"))
	      {
		if (vm.count("shift"))
		  {
		    throw UsageException("shift and expectation options are mutually exclusive!");
		  }
		if (vm.count("density") && density)
		  {
		    cerr << "Warning: density option ignored, because expectation option triggers a completely dense storage\n";
		  }
		if (secondVertexDimension)
		  {
		    throw UsageException("communities and expectation options are currently mutually exclusive");
		    // TODO: instead of above instruction
		    // roughTensor = new DenseRoughGraphTensor(tensorFileName.c_str(), vm["tds"].as<string>().c_str(), vm["tes"].as<string>().c_str(), vm.count("boolean"), firstVertexDimension, secondVertexDimension, verboseStep);
		  }
		else
		  {
		    roughTensor = new DenseRoughTensor(tensorFileName.c_str(), vm["tds"].as<string>().c_str(), vm["tes"].as<string>().c_str(), vm.count("boolean"), verboseStep);
		  }
	      }
	    else
	      {
		if (vm.count("shift"))
		  {
		    if (vm["shift"].as<float>() < 0 || vm["shift"].as<float>() >= 1)
		      {
			throw UsageException("shift option should provide a float in [0, 1[!");
		      }
		    if (secondVertexDimension)
		      {
			roughTensor = AbstractRoughGraphTensor::makeRoughTensor(tensorFileName.c_str(), vm["tds"].as<string>().c_str(), vm["tes"].as<string>().c_str(), density, vm["shift"].as<float>(), vm.count("boolean"), firstVertexDimension, secondVertexDimension, verboseStep);
		      }
		    else
		      {
			roughTensor = AbstractRoughTensor::makeRoughTensor(tensorFileName.c_str(), vm["tds"].as<string>().c_str(), vm["tes"].as<string>().c_str(), density, vm["shift"].as<float>(), vm.count("boolean"), verboseStep);
		      }
		  }
		else
		  {
		    if (secondVertexDimension)
		      {
			roughTensor = AbstractRoughGraphTensor::makeRoughTensor(tensorFileName.c_str(), vm["tds"].as<string>().c_str(), vm["tes"].as<string>().c_str(), density, vm.count("boolean"), firstVertexDimension, secondVertexDimension, verboseStep);
		      }
		    else
		      {
			roughTensor = AbstractRoughTensor::makeRoughTensor(tensorFileName.c_str(), vm["tds"].as<string>().c_str(), vm["tes"].as<string>().c_str(), density, vm.count("boolean"), verboseStep);
		      }
		  }
	      }
	    if (isModifyingOrGrowing)
	      {
		ModifiedPattern::setContext(roughTensor, 1073741824 * vm["remember"].as<float>(), nbOfJobs, vm.count("ns"));
		if (verboseStep)
		  {
		    cout << "\rShifting tensor: done.\n";
		  }
		AbstractRoughTensor::setOutput(outputFileName.c_str(), vm["ods"].as<string>().c_str(), vm["oes"].as<string>().c_str(), vm["sp"].as<string>().c_str(), vm["ss"].as<string>().c_str(), vm["ap"].as<string>().c_str(), vm["rp"].as<string>().c_str(), vm.count("pl"), vm.count("ps"), vm.count("pa")); // also prints the numeric precision and/or the tensor shifting time if NUMERIC_PRECISION and/or DETAILED_TIME defined
		if (verboseStep)
		  {
		    cout << "Getting and modifying initial patterns ..." << flush;
		  }
		if (AbstractRoughTensor::isDirectOutput())
		  {
		    roughTensor->setNoSelection();
		  }
	      }
	    else
	      {
		if (verboseStep)
		  {
		    cout << "\rShifting tensor: done.\n";
		  }
		AbstractRoughTensor::setOutput(outputFileName.c_str(), vm["ods"].as<string>().c_str(), vm["oes"].as<string>().c_str(), vm["sp"].as<string>().c_str(), vm["ss"].as<string>().c_str(), vm["ap"].as<string>().c_str(), vm["rp"].as<string>().c_str(), vm.count("pl"), vm.count("ps"), vm.count("pa")); // also prints the numeric precision and/or the tensor shifting time if NUMERIC_PRECISION and/or DETAILED_TIME defined
		if (verboseStep)
		  {
		    cout << "Getting patterns ..." << flush;
		  }
#ifdef NB_OF_PATTERNS
		ModifiedPattern::setNoOutputPattern();
#endif
	      }
	    isRSSPrinted = vm.count("pr");
	    isGrow = vm.count("grow");
#ifdef DETAILED_TIME
	    startingPoint = steady_clock::now();
#endif
	  }
	catch (unknown_option& e)
	  {
	    cerr << "Unknown option!\n";
	    return EX_USAGE;
	  }
	catch (UsageException& e)
	  {
	    cerr << e.what() << '\n';
	    return EX_USAGE;
	  }
	catch (NoInputException& e)
	  {
	    cerr << e.what() << '\n';
	    return EX_NOINPUT;
	  }
	catch (DataFormatException& e)
	  {
	    cerr << e.what() << '\n';
	    return EX_DATAERR;
	  }
	catch (NoOutputException& e)
	  {
	    cerr << e.what() << '\n';
	    return EX_CANTCREAT;
	  }
	if (isModifyingOrGrowing)
	  {
	    threads.reserve(nbOfJobs);
	    if (isGrow)
	      {
		if (secondVertexDimension)
		  {
		    do
		      {
			threads.emplace_back(ModifiedGraphPattern::grow);
		      }
		    while (--nbOfJobs);
		  }
		else
		  {
		    do
		      {
			threads.emplace_back(ModifiedPattern::grow);
		      }
		    while (--nbOfJobs);
		  }
	      }
	    else
	      {
		if (secondVertexDimension)
		  {
		    do
		      {
			threads.emplace_back(ModifiedGraphPattern::modify);
		      }
		    while (--nbOfJobs);
		  }
		else
		  {
		    do
		      {
			threads.emplace_back(ModifiedPattern::modify);
		      }
		    while (--nbOfJobs);
		  }
	      }
	  }
      }
      if (ConcurrentPatternPool::readFromFile())
	{
	  patternFileReader.read(maxNbOfInitialPatterns);
	}
    }
    if (isModifyingOrGrowing)
      {
	if (verboseStep)
	  {
	    if (verboseStep > 0)
	      {
		thread(ConcurrentPatternPool::printProgressionOnSTDIN, verboseStep).detach();
	      }
	    else
	      {
		cout << "\rGetting initial patterns: done.           \nModifying patterns ... " << flush;
	      }
	    for (thread& t : threads)
	      {
		t.join();
	      }
	    ModifiedPattern::insertCandidateVariables();
	    if (AbstractRoughTensor::isDirectOutput())
	      {
		cout << "\rModifying patterns: " << ModifiedPattern::getNbOfOutputPatterns() << " patterns with locally maximal explanatory powers.\n";
	      }
	    else
	      {
		cout << "\rModifying patterns: " << AbstractRoughTensor::candidateVariables.size() << " patterns with locally maximal explanatory powers.\n";
	      }
	  }
	else
	  {
	    for (thread& t : threads)
	      {
		t.join();
	      }
	    ModifiedPattern::insertCandidateVariables();
	  }
#ifdef DETAILED_TIME
#ifdef GNUPLOT
	cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
	cout << "Explanatory power maximization time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
#endif
      }
    else
      {
	ConcurrentPatternPool::moveTo(AbstractRoughTensor::candidateVariables);
	if (verboseStep)
	  {
	    cout << "\rGetting patterns: " << AbstractRoughTensor::candidateVariables.size() << " patterns.\n";
	  }
#ifdef DETAILED_TIME
#ifdef GNUPLOT
	cout << "\t0";
#else
	cout << "Explanatory power maximization time: 0s\n";
#endif
#endif
      }
  }
  if (AbstractRoughTensor::candidateVariables.empty() || RankPatterns::rank(roughTensor, verboseStep, maxSelectionSize, selectionCriterion, isRSSPrinted))
    {
#ifdef GNUPLOT
#ifdef NB_OF_PATTERNS
      cout << '\t' << ModifiedPattern::getNbOfOutputPatterns() << "\t0";
#endif
#ifdef NUMERIC_PRECISION
      cout << "\t0";
#endif
#ifdef DETAILED_TIME
      cout << "\t0\t0";
#endif
#else
#ifdef NB_OF_PATTERNS
      cout << "Nb of patterns candidates for selection: " << ModifiedPattern::getNbOfOutputPatterns() << '\n';
#endif
#ifdef NUMERIC_PRECISION
      cout << "Numeric precision: 0\n";
#endif
#ifdef DETAILED_TIME
      cout << "Tensor reduction time: 0s\n";
#endif
#ifdef NB_OF_PATTERNS
      cout << "Nb of selected patterns: 0\n";
#endif
#ifdef DETAILED_TIME
      cout << "Selecting time: 0s\n";
#endif
#endif
    }
  delete roughTensor;
#ifdef TIME
  AbstractRoughTensor::printCurrentDuration();
#endif
#if defined GNUPLOT && (defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined DETAILED_TIME || defined TIME)
  cout << '\n';
#endif
  return EX_OK;
}
