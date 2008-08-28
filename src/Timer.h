
#ifndef TIMER_H
#define TIMER_H

#include <unistd.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>


/** A simple timer class. 
 * This class tracks both wall clock time as well as cpu time (as reported by
 * the times() function.   This is a simple class that collects code that I
 * have been using in multiple locations for a long time.  */
class Timer {
  public:
    enum FUNCTION {
        CUMMULATIVE, /* Tracks the total time between all pairs of start()/stop(). */
        SIMPLE /* Only tracks the time between start() and stop(). */
    };

    /** Constructor defaults to a clean (zeroed) SIMPLE timer. */
    Timer(const enum FUNCTION & f = SIMPLE) { zero(); function = f; }

    /** Zero all timing buffers. This is not really necessary to do for SIMPLE
     * timers. */
    void zero() {
        memset(tv,0,2*sizeof(struct timeval));
        memset(&ti,0,sizeof(struct tms));
        memset(&tf,0,sizeof(struct tms));
        dt = 0;
        dt_cpu_time = 0;
        N_start = N_stop = 0;
    }

    /** Start the timer.  This essentially records the current time of day and
     * the current cpu usage time. */
    inline void start() {
        N_start++;
        times(&ti);
        gettimeofday(&tv[0],NULL);
    }

    /** Stop the timer.  This records the current time of day and the current
     * cpu usage time.  the calcualte() function is then called which
     * subtracts the stored value from start() from the results stored in this
     * function. The final results of the timer are placed in dt and
     * dt_cput_time by calculate(). */
    inline void stop() {
        gettimeofday(&tv[1],NULL);
        times(&tf);
        N_stop++;

        calculate();
    }

    /** Subtracts the stored values from start() from the stored values from
     * stop(). The final results of the timer are placed in dt and
     * dt_cput_time. */ 
    inline void calculate() {
        struct timeval dtv;
        Timer::subtime(tv[0],tv[1],dtv);

        double a_dt = dtv.tv_sec + double(dtv.tv_usec)*1e-6L;
        double a_dt_cpu_time = 
             ( ( (tf.tms_utime + tf.tms_stime) - (ti.tms_utime + ti.tms_stime) )
               * seconds_per_clock_tick
             );

        if (function == CUMMULATIVE) {
            dt += a_dt;
            dt_cpu_time += a_dt_cpu_time;
        } else {
            dt = a_dt;
            dt_cpu_time = a_dt_cpu_time;
        }
    }

    /** Utility function to help subtract the total time in struct timeval. */
    static void subtime(struct timeval &ti, struct timeval &tf, struct timeval &result) {
        if (tf.tv_usec < ti.tv_usec) {
            tf.tv_usec += 1000000;
            tf.tv_sec--;
        }
        result.tv_sec = tf.tv_sec - ti.tv_sec;
        result.tv_usec = tf.tv_usec - ti.tv_usec;
    }

    double dt_avg() const {
        return dt / ((double)N_start);
    }

    double dt_cpu_time_avg() const {
        return dt_cpu_time / ((double)N_start);
    }

    /* ***** storage ***** */

    /** Type of timer that this is. */
    enum FUNCTION function;

    /** Buffers for wall-clock-time measurements. */
    struct timeval tv[2];

    /** Buffers for cpu_time measurements. */
    struct tms ti, tf;

    /** The final result of the wall-clock-time measurement. */
    double dt;

    /** The final result of the cpu-time measurement. */
    double dt_cpu_time;

    /** Total number of calls to start(). */
    int N_start;

    /** Total number of calls to stop(). */
    int N_stop;

    /** Fraction/Number of seconds per each clock tick as reported by
     * sysconf(_SC_CLK_TCK). */
    static double seconds_per_clock_tick;
};

double Timer::seconds_per_clock_tick = 1.0 / sysconf(_SC_CLK_TCK);

std::ostream & operator<< (std::ostream & out, const Timer & t ) {
    if (t.function == Timer::CUMMULATIVE) {
        out << t.dt_avg() << '\t'
            << t.dt_cpu_time_avg() << '\t';
    }
    out << t.dt << '\t' << t.dt_cpu_time;
    return out;
}

#endif // TIMER_H
