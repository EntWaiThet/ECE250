#include "time_series.hpp"

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

int main() {
    // The program reads commands from stdin and produces outputs, cmd is short for command 
    std::unique_ptr<TimeSeries> ts; 
    //the currently loaded time series is stored using a smart pointer
    //unique ptr is used to avoide manual delete

    std::cout << std::setprecision(15);

    std::string cmd;
    //while loop is used to keep reading one command at a time until EXIT, then it breaks
    while (std::cin >> cmd) {
        if (cmd == "EXIT") {
            // EXIT produces no output 
            break;
        }

        // Loads a CSV file and initializes the time series object(ts)
        if (cmd == "LOAD_P1") {
    std::string filename;
    std::cin >> filename;

    ts = std::make_unique<TimeSeries>();
    bool ok = ts->load_from_csv(filename);
        //load_from_csv tells us if loading succeeded or not
        //if it fails, we reset the time series object(ts)
    if (ok) {
        std::cout << "success\n";
    } else {
        ts.reset();            
        std::cout << "failure\n";
    }
}

        else if (cmd == "PRINT_P1") {
            //prints the stored (year,value) pairs in the required format
            //prints failure if there's no data loaded
            if (!ts || !ts->has_valid_data()) {
                std::cout << "failure\n"; 
            } else {
                ts->print();  
            }
        }
        //add a new pair of (year, value) if the data doesn't exist alreadt
        //output success if inserted, failure otherwise
        else if (cmd == "ADD_P1") {
            int Y;
            double D;
            std::cin >> Y >> D;

            if (!ts) ts = std::make_unique<TimeSeries>(); //make_unique is used instead of new operator directly
            //it provides cleaner syntax and ensure no memory leak

            if (ts->add(Y, D)) {
                std::cout << "success\n"; 
            } else {
                std::cout << "failure\n"; 
            }
        }
        //update an existing year with a new value
        else if (cmd == "UPDATE_P1") {
            int Y;
            double D;
            std::cin >> Y >> D;
            //update won't work if there's no series yet
            if (!ts) {
                std::cout << "failure\n";
                continue;
            }

            if (ts->update(Y, D)) {
                std::cout << "success\n"; 
            } else {
                std::cout << "failure\n"; 
            }
        }
        //compute the mean value of the stored data
        else if (cmd == "MEAN_P1") {
            if (!ts || !ts->has_valid_data()) {
                std::cout << "failure\n"; 
            } else {
                double m = ts->mean();
                std::cout << "mean is " << m << "\n"; 
            }
        }
        //check whether the series values are monotonic
        else if (cmd == "MONOTONIC_P1") {
            if (!ts || !ts->has_valid_data()) {
                std::cout << "failure\n"; 
            } else if (ts->is_monotonic()) {
                std::cout << "series is monotonic\n"; 
            } else {
                std::cout << "series is not monotonic\n"; 
            }
        }
        //compute the least-squares best fit line y = m*x + b using (year,value)
        //prints failure if there's not sufficient data to calculate
        else if (cmd == "FIT_P1") {
            if (!ts || !ts->has_valid_data()) {
                std::cout << "failure\n";
                continue;
            }

            double m = 0.0, b = 0.0;
            if (!ts->best_fit(m, b)) {
                std::cout << "failure\n"; 
            } else {
                std::cout << "slope is " << m << " intercept is " << b << "\n"; 
            }
        }
        //if an unknown command appears, it will be ignored
    }

    return 0;
}
