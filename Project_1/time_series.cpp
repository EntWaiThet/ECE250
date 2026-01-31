#include "time_series.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

//constructor
//initializes an empty time series with a small initial capacity
//and allocate years_ and data_ arrays of size capacity_

TimeSeries::TimeSeries()
    : series_name_("")
    , series_code_("")
    , years_(nullptr)
    , data_(nullptr)
    , size_(0)
    , capacity_(2)  //capacity starts at 2 
{
    years_ = new int[capacity_];
    data_  = new double[capacity_];
}

//destructor
//delete, clear the dynamic arrays

TimeSeries::~TimeSeries() {
    delete[] years_;
    delete[] data_;
}

//copy constructor, creates a deep copy contructor
//it's to make sure that copying would not result in multiple objects sharing the same memory
TimeSeries::TimeSeries(const TimeSeries& other)
    : series_name_(other.series_name_)
    , series_code_(other.series_code_)
    , years_(nullptr)
    , data_(nullptr)
    , size_(other.size_)
    , capacity_(other.capacity_)
{
    years_ = new int[capacity_];
    data_  = new double[capacity_];
    for (int i = 0; i < size_; ++i) {
        years_[i] = other.years_[i];
        data_[i]  = other.data_[i];
    }
}
//assignment operator
//assignment operator allocates new memory for the target object,
//copies the source object’s data into it, 
//frees the target’s previously allocated memory, 
//and then updates its internal pointers

TimeSeries& TimeSeries::operator=(const TimeSeries& other) {
    if (this == &other) return *this;

    int* new_years = new int[other.capacity_];
    double* new_data = new double[other.capacity_];

    for (int i = 0; i < other.size_; ++i) {
        new_years[i] = other.years_[i];
        new_data[i]  = other.data_[i];
    }

    delete[] years_;
    delete[] data_;

    years_ = new_years;
    data_ = new_data;
    series_name_ = other.series_name_;
    series_code_ = other.series_code_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    return *this;
}

// ---------- resizing helpers ----------

//resize_to(new_capacity)
//allocates new arrays of size new_capacity and copies existing elements
//ensure a minimum capacity (2) to keep the structure usable

void TimeSeries::resize_to(int new_capacity) {
    if (new_capacity < 2) new_capacity = 2; // capacity never below 2 

    int* new_years = new int[new_capacity];
    double* new_data = new double[new_capacity];

    for (int i = 0; i < size_; ++i) {
        new_years[i] = years_[i];
        new_data[i]  = data_[i];
    }

    delete[] years_;
    delete[] data_;

    years_ = new_years;
    data_  = new_data;
    capacity_ = new_capacity;
}

void TimeSeries::expand_if_full() {
    if (size_ == capacity_) {                  //when full, double the capacity
        resize_to(capacity_ * 2);
    }
}

void TimeSeries::shrink_if_quarter() {
    //when size == capacity/4, halve (min 2) 
    if (capacity_ > 2 && size_ == capacity_ / 4) {
        resize_to(capacity_ / 2);
    }
}

// ---------- search / insertion helpers ----------
//CITATION:
// Helper functions `find_year_index' and 'insertion_index' adapted
// with assistance from OpenAI ChatGPT.

int TimeSeries::find_year_index(int year) const {
    for (int i = 0; i < size_; ++i) {
        if (years_[i] == year) return i;
    }
    return -1;
}

int TimeSeries::insertion_index(int year) const {
    int i = 0;
    while (i < size_ && years_[i] < year) {
        ++i;
    }
    return i;
}

// ---------- public operations ----------

bool TimeSeries::has_valid_data() const {
    return size_ > 0;
}

bool TimeSeries::add(int year, double value) {
    //ADD_P1: only if no valid data exists for that year 
    if (value < 0) return false;               //treat negative as invalid input for ADD
    if (find_year_index(year) != -1) return false;

    expand_if_full();

    int pos = insertion_index(year);           //keep sorted by year 
    for (int i = size_; i > pos; --i) {
        years_[i] = years_[i - 1];
        data_[i]  = data_[i - 1];
    }

    years_[pos] = year;
    data_[pos]  = value;
    ++size_;
    return true;
}

bool TimeSeries::remove(int year) {
    int idx = find_year_index(year);
    if (idx == -1) return false;

    for (int i = idx; i < size_ - 1; ++i) {
        years_[i] = years_[i + 1];
        data_[i]  = data_[i + 1];
    }
    --size_;
    shrink_if_quarter();
    return true;
}

bool TimeSeries::update(int year, double value) {
    //UPDATE_P1: only if year exists with valid data; if value < 0 remove 
    int idx = find_year_index(year);
    if (idx == -1) return false;

    if (value < 0) {
        return remove(year);
    }

    data_[idx] = value;
    return true;
}

// ---------- printing ----------

void TimeSeries::print() const {
    //PRINT_P1: print (year,data) pairs with spaces between; failure if no valid data 
    for (int i = 0; i < size_; ++i) {
        if (i > 0) std::cout << " ";
        std::cout << "(" << years_[i] << "," << data_[i] << ")";
    }
    std::cout << "\n";
}

// ---------- stats (required headers) ----------

double TimeSeries::mean() const {
    //returns 0 if no valid data 
    if (size_ == 0) return 0.0;

    double sum = 0.0;
    for (int i = 0; i < size_; ++i) {
        
        sum += data_[i];
    }
    return sum / static_cast<double>(size_);
}

bool TimeSeries::is_monotonic() const {
    //returns false if no valid data 
    if (size_ == 0) return false;
    if (size_ == 1) return true; //1 point is monotonic 

    bool nondecreasing = true;
    bool nonincreasing = true;

    for (int i = 1; i < size_; ++i) {
        if (data_[i] < data_[i - 1]) nondecreasing = false;
        if (data_[i] > data_[i - 1]) nonincreasing = false;
        if (!nondecreasing && !nonincreasing) return false;
    }
    return true;
}

bool TimeSeries::best_fit(double& m, double& b) const {
    //needs at least 2 points of valid data for command-level success 
    if (size_ == 0) {
        m = 0.0;
        b = 0.0;
        return false;
    }
    if (size_ == 1) {
        m = 0.0;
        b = 0.0;
        return false;
    }

    //formulas given 
    const double N = static_cast<double>(size_);
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;

    for (int i = 0; i < size_; ++i) {
        double x = static_cast<double>(years_[i]);
        double y = data_[i];
        sum_x  += x;
        sum_y  += y;
        sum_xy += x * y;
        sum_x2 += x * x;
    }

    double denom = (N * sum_x2) - (sum_x * sum_x);
    if (std::fabs(denom) < 1e-12) {
        m = 0.0;
        b = 0.0;
        return false;
    }

    m = ((N * sum_xy) - (sum_x * sum_y)) / denom;
    b = (sum_y - (m * sum_x)) / N;
    return true;
}

// ---------- loading from CSV ----------

//parses a string into a double, but only succeeds if the entire string is numeric
//this prevents partial parses like "12abc" from being accepted

static bool parse_double_strict(const std::string& s, double& out) {
    //accepts typical numeric strings; rejects empty/non-numeric
    std::stringstream ss(s);
    ss >> out;
    return ss && ss.eof();
}

//CITATION:
//while running test11.in, I keep getting 32 for mean while the real value is 32.5
//I supposed that was an error with my load_from_csv function such as not loading all data
//helper function `trim()` and CSV parsing strategy adapted
//with assistance from OpenAI ChatGPT

 static void trim(std::string& s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\t'))
        s.pop_back();
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
        ++i;
    s.erase(0, i);
}

bool TimeSeries::load_from_csv(const std::string& filename) {
    series_name_.clear();
    series_code_.clear();
    size_ = 0;
    resize_to(2);
    
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;

    if (!std::getline(file, line)) return false;

    if (line.find("Country Name") == std::string::npos) {
        file.clear();
        file.seekg(0);
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        series_name_.clear();
        series_code_.clear();
        size_ = 0;
        resize_to(2);

        std::stringstream ss(line);
        std::string field;

        std::string country_name, country_code;
        if (!std::getline(ss, country_name, ',')) continue;
        if (!std::getline(ss, country_code, ',')) continue;
        if (!std::getline(ss, series_name_, ',')) continue;
        if (!std::getline(ss, series_code_, ',')) continue;

        int year = 1960;

        while (std::getline(ss, field, ',')) {
            trim(field);

            double val = 0.0;
            if (field == "-1" || field == ".." || field.empty()) {
                //missing //should only be -1??, will edit later
            } else if (parse_double_strict(field, val)) {
                if (val >= 0) {
                    add(year, val);
                }
            }
            ++year;
        }
        
        
    }
        return true;

}