// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Based on Standard reporter by nonius, modified by Axel Menzel

#ifndef NONIUS_REPORTERS_HTML_GROUP_REPORTER_HPP
#define NONIUS_REPORTERS_HTML_GROUP_REPORTER_HPP

#include <nonius/nonius.h++>

#include <ratio>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <memory>
#include <ctime>

namespace nonius {
    struct collected_data
    {
         environment_estimate<fp_seconds> m_mean;
         sample_analysis<fp_seconds>      m_analysis;
         std::vector<fp_seconds>          m_sample_data;
    };

    struct html_group_reporter : reporter {

    void set_current_group_name(std::string benchmark_group, std::string tooltip = std::string()){

        m_all_benchmarks.push_back(std::make_pair(group_data{benchmark_group, tooltip}, benchmark_list()));
        current_benchmark_group = std::move(benchmark_group);
    }

    void set_barchart_height(std::string height) {
        m_barchart_height = height;
    }

    void set_scatterchart_height(std::string height) {
        m_scatterchart_height = height;
    }

    void set_output_file(std::string file_path) {
        m_output_file_path = file_path;
    }

    void set_enable_print_datetime(bool enable) {
        m_can_print_datetime = enable;
    }

    void set_place_legend_below_chart(bool enable) {
        place_legend_below = enable;
    }

    void generate_report() const {

        const std::string file_name = gen_unique_filename(m_output_file_path);
        std::cout << "\ngenerating HTML report: '" << file_name << "'\n";

        auto&& templ = template_string();

        const auto samples_magnitude = ideal_magnitude_from_samples();
        const auto mean_magnitude = ideal_magnitude_from_mean_analysis();

        cpptempl::data_map map;

        map["page_title"]           = escape(title);
        map["height_barchart"]      = m_barchart_height;
        map["height_scatterchart"]  = m_scatterchart_height;
        map["barchart_title"]       = escape(title);
        map["barchart_subtitle"]    = m_can_print_datetime ? escape(get_current_datetime()) : std::string("");
        map["barchart_unit"]        = detail::units_for_magnitude(mean_magnitude);
        map["place_legend_below"]   = place_legend_below;

        for(const auto& bench_group : m_all_benchmarks) {
            cpptempl::data_map benchmark_group;
            benchmark_group["title"] = escape(bench_group.first.title);
            benchmark_group["title_tooltip"] = escape(bench_group.first.tooltip);
            benchmark_group["unit"] = detail::units_for_magnitude(samples_magnitude);
            cpptempl::data_map bench_list_data;

            for(const auto& bench_list : bench_group.second) {
                cpptempl::data_map benchmark_data;
                benchmark_data["name"]              = escape(bench_list.first);
                benchmark_data["mean"]              = bench_list.second.m_analysis.mean.point.count() * mean_magnitude;
                benchmark_data["mean_unit"]         = detail::units_for_magnitude(mean_magnitude);
                benchmark_data["mean_lb"]           = bench_list.second.m_analysis.mean.lower_bound.count() * mean_magnitude;
                benchmark_data["mean_lb_unit"]      = detail::units_for_magnitude(mean_magnitude);
                benchmark_data["mean_ub"]           = bench_list.second.m_analysis.mean.upper_bound.count() * mean_magnitude;
                benchmark_data["mean_ub_unit"]      = detail::units_for_magnitude(mean_magnitude);
                benchmark_data["mean_ci"]           = truncate(bench_list.second.m_analysis.mean.confidence_interval);

                const auto std_dev_magnitude = detail::get_magnitude(bench_list.second.m_analysis.standard_deviation.point);
                benchmark_data["std_dev"]           = bench_list.second.m_analysis.standard_deviation.point.count() * std_dev_magnitude;
                benchmark_data["std_dev_unit"]      = detail::units_for_magnitude(std_dev_magnitude);
                const auto std_dev_lb_magnitude = detail::get_magnitude(bench_list.second.m_analysis.standard_deviation.lower_bound);
                benchmark_data["std_dev_lb"]        = bench_list.second.m_analysis.standard_deviation.lower_bound.count() * std_dev_lb_magnitude;
                benchmark_data["std_dev_lb_unit"]   = detail::units_for_magnitude(std_dev_lb_magnitude);
                const auto std_dev_ub_magnitude = detail::get_magnitude(bench_list.second.m_analysis.standard_deviation.upper_bound);
                benchmark_data["std_dev_ub"]        = bench_list.second.m_analysis.standard_deviation.upper_bound.count() * std_dev_ub_magnitude;
                benchmark_data["std_dev_ub_unit"]   = detail::units_for_magnitude(std_dev_ub_magnitude);
                benchmark_data["std_dev_ci"]        = truncate(bench_list.second.m_analysis.standard_deviation.confidence_interval);

                benchmark_data["outliers"]          = get_outliers_analysis(bench_list.second.m_analysis);
                benchmark_data["outliers_descr"]    = get_outliers_effect(bench_list.second.m_analysis);

                for(auto e : bench_list.second.m_sample_data) {
                    benchmark_data["times"].push_back(e.count() * samples_magnitude);
                }
                benchmark_group["data"].push_back(benchmark_data);
            }
             map["benchmark_groups"].push_back(benchmark_group);
        }

        std::ofstream ofs(file_name);

        cpptempl::parse(ofs, templ, map);
        ofs << std::flush;

        std::cout << "done\n";
    }

    private:
        std::string description() override {
            return "outputs an HTML file with an interactive chart of all grouped benchmarks";
        }

        static std::string get_outliers_analysis(sample_analysis<fp_seconds> const& analysis){
            auto o = analysis.outliers;
            return std::string("Found ") + std::to_string(o.total()) + std::string(" outliers among ") + std::to_string(o.samples_seen) + std::string(" samples (") + detail::percentage_ratio(o.total(), o.samples_seen) + std::string(")");
        }

        static std::string get_outliers_effect(sample_analysis<fp_seconds> const& analysis){
            const char* effect;
            if(analysis.outlier_variance < 0.01) {
                effect = "unaffected";
            } else if(analysis.outlier_variance < 0.1) {
                effect = "slightly inflated";
            } else if(analysis.outlier_variance < 0.5) {
                effect = "moderately inflated";
            } else {
                effect = "severely inflated";
            }
            //std::string tmp = std::string("variance introduced by outliers: ") + detail::percentage(analysis.outlier_variance) + "\n";
            return std::string("variance is ") + std::string(effect) + std::string(" by outliers");
        }

        void do_configure(configuration& cfg) override {
            n_samples = cfg.samples;
            verbose = cfg.verbose;
            summary = cfg.summary;
            n_resamples = cfg.resamples;
            title = cfg.title;
        }

        static std::string const& template_string() {
            static char const* template_parts[] = {
// generated content broken into pieces because MSVC is in the 1990s.
#include <nonius/html_group_reporter_template.h>
            };
            static std::string const the_template = []() -> std::string {
                std::string s;
                for(auto part : template_parts) {
                    s += part;
                }
                return s;
            }();
            return the_template;
        }

        void do_warmup_start() override {
            if(verbose) std::cout << "warming up\n";
        }
        void do_estimate_clock_resolution_start() override {
            if(verbose) std::cout << "estimating clock resolution\n";
        }
        void do_estimate_clock_resolution_complete(environment_estimate<fp_seconds> estimate) override {
            if(!summary) {
                if(!verbose) std::cout << "clock resolution: ";
                print_environment_estimate(estimate, estimate.outliers.samples_seen + 2);
            }
        }

        void do_estimate_clock_cost_start() override {
            if(verbose) std::cout << "estimating cost of a clock call\n";
        }
        void do_estimate_clock_cost_complete(environment_estimate<fp_seconds> estimate) override {
            if(verbose) print_environment_estimate(estimate, estimate.outliers.samples_seen);
        }

        void do_benchmark_start(std::string const& name) override {
            std::cout << '\n';
            if(!summary) std::cout << "benchmarking ";
            std::cout << "\"" << current_benchmark_group << " - ";
            std::cout << name << "\"\n";
            current_benchmark = name;
            m_all_benchmarks.back().second.push_back(std::make_pair(name, collected_data()));
        }

        void do_measurement_start(execution_plan<fp_seconds> plan) override {
            std::cout << std::setprecision(7);
            std::cout.unsetf(std::ios::floatfield);
            if(!summary) std::cout << "collecting " << n_samples << " samples, " << plan.iterations_per_sample << " iterations each, in estimated " << detail::pretty_duration(plan.estimated_duration) << "\n";
        }

        void do_measurement_complete(std::vector<fp_seconds> const& samples) override {
            set_bench_data(samples);
        }

        void do_analysis_start() override {
            if(verbose) std::cout << "bootstrapping with " << n_resamples << " resamples\n";
        }
        void do_benchmark_failure(std::exception_ptr) override {
            error_stream() << current_benchmark << " failed to run successfully\n";
            std::cout << "benchmark aborted\n";
        }
        void do_analysis_complete(sample_analysis<fp_seconds> const& analysis) override {
            set_analysis_data(analysis);
            print_statistic_estimate("mean", analysis.mean);
            print_statistic_estimate("std dev", analysis.standard_deviation);
            if(!summary) print_outliers(analysis.outliers);
            if(verbose) std::cout << "variance introduced by outliers: " << detail::percentage(analysis.outlier_variance) << "\n";
            const char* effect;
            if(analysis.outlier_variance < 0.01) {
                effect = "unaffected";
            } else if(analysis.outlier_variance < 0.1) {
                effect = "slightly inflated";
            } else if(analysis.outlier_variance < 0.5) {
                effect = "moderately inflated";
            } else {
                effect = "severely inflated";
            }
            std::cout << "variance is " << effect << " by outliers\n";
        }

        collected_data do_get_collected_data() const {
            return collected_data();
        }

        void print_environment_estimate(environment_estimate<fp_seconds> e, int iterations) {
            std::cout  << std::setprecision(7);
            std::cout.unsetf(std::ios::floatfield);
            std::cout  << "mean is " << detail::pretty_duration(e.mean) << " (" << iterations << " iterations)\n";
            if(verbose) print_outliers(e.outliers);
        }
        void print_outlier_count(const char* description, int count, int total) {
            if(count > 0) std::cout << "  " << count << " (" << detail::percentage_ratio(count, total) << ") " << description << "\n";
        }
        void print_outliers(outlier_classification o) {
            std::cout << "found " << o.total() << " outliers among " << o.samples_seen << " samples (" << detail::percentage_ratio(o.total(), o.samples_seen) << ")\n";
            if(verbose) {
                std::cout << "low severe" << o.low_severe << " " << o.samples_seen;
                std::cout << "low mild" << o.low_mild << " " << o.samples_seen;
                std::cout << "high mild" << o.high_mild << " " << o.samples_seen;
                std::cout << "high severe" << o.high_severe << " " << o.samples_seen;
            }
        }
        void print_statistic_estimate(const char* name, estimate<fp_seconds> estimate) {
            std::cout << std::setprecision(7);
            std::cout.unsetf(std::ios::floatfield);
            std::cout << name << ": " << detail::pretty_duration(estimate.point);
            if(!summary) {
                std::cout << ", lb " << detail::pretty_duration(estimate.lower_bound)
                         << ", ub " << detail::pretty_duration(estimate.upper_bound)
                         << ", ci " << std::setprecision(3) << estimate.confidence_interval;
            }
            std::cout << "\n";
        }

        static double truncate(double x) {
            return static_cast<int>(x * 1000.) / 1000.;
        }

        double ideal_magnitude_from_samples() const {
            std::vector<fp_seconds> mins;
            for(auto bench_groups : m_all_benchmarks) {
                for(auto bench_list : bench_groups.second) {
                    if (!bench_list.second.m_sample_data.empty())
                        mins.push_back(*std::min_element(bench_list.second.m_sample_data.begin(), bench_list.second.m_sample_data.end()));
                }
            }
            auto min = mins.empty() ? fp_seconds() : *std::min_element(mins.begin(), mins.end());
            return detail::get_magnitude(min);
        }

        double ideal_magnitude_from_mean_analysis() const {
            std::vector<fp_seconds> mins;
            for(auto bench_groups : m_all_benchmarks) {
                for(auto bench_list : bench_groups.second) {
                    mins.push_back(bench_list.second.m_analysis.mean.point);
                }
            }
            auto min = mins.empty() ? fp_seconds() : *std::min_element(mins.begin(), mins.end());
            return detail::get_magnitude(min);
        }

        static std::string escape(std::string const& source) {
            static const std::unordered_map<char, std::string> escapes {
                { '\'', "&apos;" },
                { '"',  "&quot;" },
                { '<',  "&lt;"   },
                { '>',  "&gt;"   },
                { '&',  "&amp;"  },
                { '\\',  "\\\\"  },
                { '\n',  "\\n"   },
            };
            return detail::escape(source, escapes);
        }

        void set_bench_data(const std::vector<fp_seconds>& data)
        {
            m_all_benchmarks.back().second.back().second.m_sample_data = data;
        }

        void set_analysis_data(const sample_analysis<fp_seconds>& analysis_data)
        {
            m_all_benchmarks.back().second.back().second.m_analysis = analysis_data;
        }

        static inline bool file_exists(const std::string& name) {
            if (FILE *file = fopen(name.c_str(), "r")) {
                fclose(file);
                return true;
            } else {
                return false;
            }
        }

        static inline std::string gen_unique_filename(const std::string& filename) {
            std::string::size_type pos_dot = filename.find_last_of(".");
            const std::string file_basename = filename.substr(0, pos_dot);
            const std::string file_suffix = filename.substr(pos_dot + 1);

            std::string new_path = file_basename + "." + file_suffix;
            int counter = 1;
            while (file_exists(new_path)) {
                   new_path = new_path = file_basename + "_" + std::to_string(counter) + "." + file_suffix;
                   ++counter;
             }

            return new_path;
        }

        static inline std::string get_current_datetime() {
            auto t = std::time(nullptr);
            const auto tm = std::localtime(&t);
            char buffer[20];
            std::strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", tm);
            return buffer;
        }

        int n_samples = 0;
        int n_resamples = 0;
        bool verbose = false;
        bool summary = false;
        bool place_legend_below = false;
        std::string m_barchart_height = "580px";
        std::string m_scatterchart_height = "250px";
        bool m_can_print_datetime = true;
        std::string m_output_file_path;

        std::string title;
        std::string current_benchmark;
        std::string current_benchmark_group;

        struct group_data
        {
            std::string title;
            std::string tooltip;
        };

        typedef std::vector<std::pair<std::string, collected_data>> benchmark_list;
        std::vector<std::pair<group_data, benchmark_list>> m_all_benchmarks;

    };

    NONIUS_REPORTER("html_group", html_group_reporter);
} // namespace nonius

#endif // NONIUS_REPORTERS_HTML_GROUP_REPORTER_HPP
