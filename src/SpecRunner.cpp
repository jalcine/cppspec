/*
 * Copyright 2007 Timo Puronen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "SpecRunner.h"
#include "SpecificationRegistry.h"
#include "Reporter.h"
#include "SpecDoxReporter.h"
#include "JUnitReporter.h"
#include "CuteReporter.h"
#include "BoostTimer.h"
#include "ConsoleOutputStream.h"
#include "Needle/Binder.h"
#include <memory.h>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>


namespace CppSpec {

class ShouldBeRun {
public:
    explicit ShouldBeRun(const std::vector<std::string>& specificationsToRun) : specificationsToRun(specificationsToRun) {
    }

    bool operator()(const std::string& name) {
        return specificationsToRun.empty() || std::find(specificationsToRun.begin(), specificationsToRun.end(), name) != specificationsToRun.end();
    }

private:
    const std::vector<std::string>& specificationsToRun;
};

SpecRunner::SpecRunner() {
}

void SpecRunner::runSpecifications(int argc, char* argv[]) {
	std::vector<std::string> specificationsToRun;
	boost::program_options::options_description options("Options");
	options.add_options()
		("output,o", boost::program_options::value<std::string>(), "define output format. Allowed values: junit console cute")
		("no-logs", "do not create log files when using JUnitReporter. No effect on other reporters")
		("report-dir", boost::program_options::value<std::string>(), "directory where JUnit reports are created")
		("specification,s", boost::program_options::value<std::vector<std::string> >(&specificationsToRun), "specification to be run, if multiple specifications will be run, the option can be repeated")
		("help,h", "show help");
	boost::program_options::variables_map args;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), args);
	boost::program_options::notify(args);

	if(args.count("help")) {
		std::cout << options << std::endl;
		return;
	}

	Needle::Binder::instance().bind<Timer>(new BoostTimer());

	OutputStream* outputStream = createOutputStream(args);
	Reporter* reporter = createReporter(args, *outputStream);

	runSpecs(specificationsToRun, reporter);

	delete reporter;
	delete outputStream;
}

OutputStream* SpecRunner::createOutputStream(boost::program_options::variables_map& /*args*/) {
    return new ConsoleOutputStream();
}

Reporter* SpecRunner::createReporter(boost::program_options::variables_map& args, OutputStream& outputStream) {
    if(args.count("output")) {
    	const std::string& selectedReporter(args["output"].as<std::string>());
        if(selectedReporter == "junit") {
			if(args.count("no-logs")) {
				return new JUnitReporter();
			}
			else {
				std::string reportPath(".");
				if(args.count("report-dir")) {
					reportPath = args["report-dir"].as<std::string>();
				}
				return new JUnitReporter(reportPath);
			}
        } else if(selectedReporter == "cute"){
        	return new CuteReporter();
        }
    }
	return new SpecDoxReporter(outputStream);
}

void SpecRunner::runSpecs(const std::vector<std::string>& specificationsToRun, Reporter* reporter) {
	ShouldBeRun shouldBeRun(specificationsToRun);
	BOOST_FOREACH(Runnable* specification, SpecificationRegistry::instance().getSpecifications()) {
		if(shouldBeRun(specification->getName())) {
			(*specification)(reporter);
		}
	}
}

}