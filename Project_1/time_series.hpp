#ifndef TIMESERIES_HPP
#define TIMESERIES_HPP

#include <string>

class TimeSeries {
private:
    std::string series_name_;
    std::string series_code_;
    //dynamic arrays storing years and their data
    int* years_;
    double* data_;
    //current number of stored points and allocated capacity
    int size_;
    int capacity_;
    //resizing helpers
    void expand_if_full();
    void shrink_if_quarter();
    void resize_to(int new_capacity);
    //search/insert helpers
    int find_year_index(int year) const;        //returns index if it found an exisiting year, otherwise -1
    int insertion_index(int year) const;        //finds where a new year should be inserted to keep years sorted

public:
    TimeSeries();
    ~TimeSeries();

    TimeSeries(const TimeSeries& other);
    TimeSeries& operator=(const TimeSeries& other);

    //loading data from csv
    bool load_from_csv(const std::string& filename);

    //Commands operate on (year, data) pairs
    bool add(int year, double value);           // inserts/add a new pair of (year,value) if not existed
    bool update(int year, double value);        // update the value for an existing year
    bool remove(int year);                      // remove an existing pair of (year, value)

    //Output helpers
    bool has_valid_data() const;
    void print() const;

    //Required statistics functions 
    double mean() const;
    bool is_monotonic() const;
    bool best_fit(double& m, double& b) const;
};

#endif
