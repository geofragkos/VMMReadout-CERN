#ifndef CALIBRATION_MODULE_H
#define CALIBRATION_MODULE_H

// vmm
#include "message_handler.h"

// qt
#include <QObject>

// ------------------------------------------------------------------- //
//  Structures for holding the calib items
// ------------------------------------------------------------------- //

///////////////////////////////////////////////
// PDO calibration
///////////////////////////////////////////////
struct pdoCalibration {

    // IF YOU ADD ANY DATA MEMBERS
    // YOU MUST ADD THEM TO THE
    // CONSTRUCTOR!!!

    pdoCalibration();

    // gain range
    int gain_start;
    int gain_end;

    // threshold range
    int threshold_start;
    int threshold_end;
    int threshold_step;

    // pulser amplitude
    int pulser_start;
    int pulser_end;
    int pulser_step;

};

struct tdoCalibration {
    // IF YOU ADD ANY DATA MEMBERS
    // YOU MUST ADD THEM TO THE
    // CONSTRUCTOR!!!

    tdoCalibration();

    // number of skew steps, in intervals
    // of 3.125 ns (delays w.r.t. CKBC)
    int n_steps;

    // pulser gain
    int gain;
    // pulser amplitude
    int amplitude;
    // dac threshold
    int threshold;
};

class CalibModule : public QObject
{
    Q_OBJECT

    public :
        explicit CalibModule(QObject *parent = 0);
        virtual ~CalibModule(){};
        CalibModule& setDebug(bool dbg) { m_dbg = dbg; return *this; }
        bool dbg() { return m_dbg; }

        void LoadMessageHandler(MessageHandler& msg);
        MessageHandler& msg() { return *m_msg; }

        void setNEvents(int number) { m_events_for_loop = number; }
        int getNEvents() { return m_events_for_loop; }

        bool setChannelRange(int chan_start = 1, int chan_end = 1);
        int getChannelStart() { return m_chan_start; }
        int getChannelEnd() { return m_chan_end; }

        /////////////////////////////////
        // calibration recipes
        /////////////////////////////////

        // pdo calibration
        bool loadPDOCalibrationRecipe(pdoCalibration& calib);
        pdoCalibration& pdoCalib() { return m_pdoCalib; }
        void beginPDOCalibration();

        // tdo calibration
        bool loadTDOCalibrationRecipe(tdoCalibration& calib);
        tdoCalibration& tdoCalib() { return m_tdoCalib; }
        void beginTDOCalibration();

        bool continueLoop() { return m_continueLoop; }
        void quitLoop();

    private :
        bool m_dbg;
        MessageHandler *m_msg;

        std::vector<std::string> m_gains;
        std::vector<std::string> m_delays;

        int m_events_for_loop;
        int m_chan_start;
        int m_chan_end;

        pdoCalibration m_pdoCalib;
        tdoCalibration m_tdoCalib;

        bool m_advance;
        bool m_continueLoop;

    signals :
        void calibrationLoopState(bool);
        void setPDOCalibrationState(int,int,int); // set gain, thresh, amplitude
        void setTDOCalibrationState(int,int,int,int); // set delay, gain, amplitude, thresh
        void setChannels(int);
        void setupCalibrationConfig();
        void setCalibrationACQon(int);
        void setCalibrationACQoff();
        void endCalibrationRun();

        void calibACQcomplete();

    public slots :
        void advanceCalibLoop();
        void stopCalibrationLoop();



}; // class CalibModule


#endif
