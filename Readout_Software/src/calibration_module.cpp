// vmm
#include "calibration_module.h"

// std/stl
#include <iostream>
using namespace std;

// qt
#include <QEventLoop>


//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  pdoCalibration struct
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////
pdoCalibration::pdoCalibration()
{
    gain_start = 1;
    gain_end = 1;

    threshold_start = 200;
    threshold_end = 300;
    threshold_step = 50;

    pulser_start = 200;
    pulser_end = 300;
    pulser_step = 50;
}

//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  tdoCalibration struct
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////
tdoCalibration::tdoCalibration()
{
    n_steps = 5; // [0, 3.125, 6.25, 9.375, 12.5] ns

    gain = 1;
    amplitude = 300;
    threshold = 200;
}

//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  CalibModule
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////
CalibModule::CalibModule(QObject *parent) :
    QObject(parent),
    m_dbg(true),
    m_msg(0),
    m_events_for_loop(1000),
    m_chan_start(1),
    m_chan_end(1),
    m_advance(false),
    m_continueLoop(true)
{
    std::vector<string> gains {"0.5","1.0","3.0","4.5","6.0","9.0","12.0","16.0"};
    m_gains = gains;

    std::vector<string> delays { "0.0", "3.125", "6.25", "9.375", "12.5", "15.625", "18.75", "21.875" };
    m_delays = delays;

    connect(this, SIGNAL(calibACQcomplete()), this, SLOT(advanceCalibLoop()));
}
// ------------------------------------------------------------------------ //
void CalibModule::advanceCalibLoop()
{
    m_advance = true;
}
// ------------------------------------------------------------------------ //
void CalibModule::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ------------------------------------------------------------------------ //
bool CalibModule::setChannelRange(int start, int end)
{
    stringstream sx;

    bool ok = true;

    m_chan_start = start;
    m_chan_end = end;

    if( (m_chan_start > 63 || m_chan_start < 0) ) {
        ok = false;
        sx << "ERROR Starting channel number is invalid.\n>>Must be between (inclusive)"
           << "0 and 63";
        msg()(sx,"CalibModule::setChannelRange"); sx.str("");
    }
    if( (m_chan_end > 63 || m_chan_start < 0) ) {
        ok = false;
        sx << "ERROR Ending channel number is invalid.\n>>Must be between (inclusive)"
           << "1 and 64";
        msg()(sx,"CalibModule::setChannelRange"); sx.str("");
    }
    if( m_chan_end < m_chan_start ) {
        ok = false;
        sx << "ERROR You have set the ending channel to be lower than the starting channel.";
        msg()(sx,"CalibModule::setChannelRange"); sx.str("");
    }

    return ok;
}
// ------------------------------------------------------------------------ //
bool CalibModule::loadPDOCalibrationRecipe(pdoCalibration& calib)
{

    stringstream sx;

    bool ok = true;

    if(dbg()) {
        sx << "Loading PDO calibration recipe...";
        msg()(sx,"CalibModule::loadPDOCalibrationRecipe"); sx.str("");
    }

    if(calib.gain_start > calib.gain_end) {
        msg()("ERROR Invalid gain range","CalibModule::loadPDOCalibrationRecipe");
        ok = false;
    } 
    if(calib.threshold_start > calib.threshold_end) {
        msg()("ERROR Invalid threshold range","CalibModule::loadPDOCalibrationRecipe");
        ok = false;
    }
    if(calib.pulser_start > calib.pulser_end) {
        msg()("ERROR Invalid pulser amplitude range","CalibModule::loadPDOCalibrationRecipe");
        ok = false;
    }

    // gain ranges
    m_pdoCalib.gain_start = calib.gain_start;
    m_pdoCalib.gain_end = calib.gain_end;

    // threshold ranges
    m_pdoCalib.threshold_start = calib.threshold_start;
    m_pdoCalib.threshold_end = calib.threshold_end;
    m_pdoCalib.threshold_step = calib.threshold_step;

    // amplitude ranges
    m_pdoCalib.pulser_start = calib.pulser_start;
    m_pdoCalib.pulser_end = calib.pulser_end;
    m_pdoCalib.pulser_step = calib.pulser_step;

    return ok;
}
// ------------------------------------------------------------------------ //
bool CalibModule::loadTDOCalibrationRecipe(tdoCalibration& calib)
{
    stringstream sx;

    bool ok = true;

    if(dbg()) {
        sx << "Loading TDO calibration recipe...";
        msg()(sx,"CalibModule::loadTDOCalibrationRecipe"); sx.str("");
    }

    m_tdoCalib.n_steps = calib.n_steps;
    m_tdoCalib.gain = calib.gain;
    m_tdoCalib.amplitude = calib.amplitude;
    m_tdoCalib.threshold = calib.threshold;

    return ok;
}
// ------------------------------------------------------------------------ //
void CalibModule::stopCalibrationLoop()
{
    m_continueLoop = false;
}
// ------------------------------------------------------------------------ //
void CalibModule::quitLoop()
{
    stringstream sx;
    sx << "Exiting calibration loop...";
    msg()(sx, "CalibModule::quitLoop"); sx.str("");

    emit setCalibrationACQoff();

    emit endCalibrationRun();

    emit calibrationLoopState(false);
    
}
// ------------------------------------------------------------------------ //
void CalibModule::beginPDOCalibration()
{
    stringstream sx;

    bool ok;

    sx << "-----------------------------------\n"
       << "   * Starting PDO calibration *\n"
       << "-----------------------------------";
    msg()(sx,"CalibModule::beginPDOCalibration"); sx.str("");

    emit calibrationLoopState(true);
    m_continueLoop = true;

    int g_start = pdoCalib().gain_start;
    int g_end = pdoCalib().gain_end;

    int t_start = pdoCalib().threshold_start;
    int t_end = pdoCalib().threshold_end;
    int t_step = pdoCalib().threshold_step;

    int amp_start = pdoCalib().pulser_start;
    int amp_end = pdoCalib().pulser_end;
    int amp_step = pdoCalib().pulser_step;

    int chan_start = getChannelStart();
    int chan_end = getChannelEnd();
    int events_per_chan = getNEvents();

    for(int g = g_start; g <= g_end; g++) {
        if(!continueLoop()) { quitLoop(); return; }
        for(int t = t_start; t <= t_end; t+= t_step) {
            if(!continueLoop()) { quitLoop(); return; }
            for(int amp = amp_start; amp <= amp_end; amp += amp_step) {
                if(!continueLoop()) { quitLoop(); return; }

                sx << "-----------------------------------\n"
                   << "  PDO calibration loop update\n"
                   << " - - - - - - - - - - - - - - - - - \n"
                   << "    > Gain      : " << m_gains[g] << " mV/fC\n"
                   << "    > Threshold : " << t << "\n"
                   << "    > Amplitude : " << amp << "\n" 
                   << "-----------------------------------";
                msg()(sx,"CalibModule::beginPDOCalibration"); sx.str("");

                emit setPDOCalibrationState(g, t, amp);

                for(int ch = chan_start; ch <= chan_end; ch++) {
                    if(!continueLoop()) { quitLoop(); return; }

                    m_advance = false;
                    sx << " - - - - - - - - - - - - - - - - - \n"
                       << "    > Channel   : " << ch << "\n"
                       << " - - - - - - - - - - - - - - - - - ";
                    msg()(sx,"CalibModule::beginPDOCalibration"); sx.str("");

                    emit setChannels(ch);
                    // put delays in within MainWindow

                    emit setupCalibrationConfig();

                    emit setCalibrationACQon(events_per_chan);

                    if(!continueLoop()) { quitLoop(); return; }
                    while(!m_advance && continueLoop()) {}

                   // {
                   //     // mainwindow needs to have a signal attached to calibmodule slot
                   //     // whcih then emits 'calibACQcomplete'
                   //     // or in mainwindow just call "emit calibModule().calibACQcomplete()"
                   //     QEventLoop acqLoop;
                   //     acqLoop.connect(this, SIGNAL(calibACQcomplete()), SLOT(quit()));
                   //     acqLoop.exec();
                   // }

                emit setCalibrationACQoff();

                if(!continueLoop()) { quitLoop(); return; }
                    
                } // ch
            } // amp
        } // t
    } // g
    sx << "-----------------------------------\n"
       << "   * PDO calibration finished *\n"
       << "-----------------------------------";
    msg()(sx,"CalibModule::beginPDOCalibration"); sx.str("");
    emit endCalibrationRun();

    emit calibrationLoopState(false);

    return;

}
// ------------------------------------------------------------------------ //
void CalibModule::beginTDOCalibration()
{
    stringstream sx;

    sx << "-----------------------------------\n"
       << "   * Starting TDO calibration *\n"
       << "-----------------------------------";
    msg()(sx,"CalibModule::beginTDOCalibration"); sx.str("");

    emit calibrationLoopState(true);
    m_continueLoop = true;

    int n_steps = tdoCalib().n_steps;

    int gain = tdoCalib().gain;
    int amplitude = tdoCalib().amplitude;
    int threshold = tdoCalib().threshold;

    int chan_start = getChannelStart();
    int chan_end = getChannelEnd();
    int events_per_chan = getNEvents();

    for(int istep = 0; istep <= n_steps; istep++) {
        if(!continueLoop()) { quitLoop(); return; }

        sx << "-----------------------------------\n"
           << "  TDO calibration loop update\n"
           << " - - - - - - - - - - - - - - - - - \n"
           << "    > TP delay  : " << m_delays[istep] << " ns\n"
           << "    > Gain      : " << m_gains[gain] << " mV/fC\n"
           << "    > Threshold : " << threshold << "\n"
           << "    > Amplitude : " << amplitude << "\n" 
           << "-----------------------------------";
        msg()(sx,"CalibModule::beginTDOCalibration"); sx.str("");

        emit setTDOCalibrationState(istep, gain, amplitude, threshold);

        for(int ch = chan_start; ch <= chan_end; ch++) {
            if(!continueLoop()) { quitLoop(); return; }

            m_advance = false;
            sx << " - - - - - - - - - - - - - - - - - \n"
               << "    > Channel   : " << ch << "\n"
               << " - - - - - - - - - - - - - - - - - ";
            msg()(sx,"CalibModule::beginTDOCalibration"); sx.str("");

            emit setChannels(ch);

            emit setupCalibrationConfig();

            emit setCalibrationACQon(events_per_chan);

            if(!continueLoop()) { quitLoop(); return; }
            while(!m_advance && continueLoop()) {}

            emit setCalibrationACQoff();

            if(!continueLoop()) { quitLoop(); return; }

        } // ch
    } // istep

    sx << "-----------------------------------\n"
       << "   * TDO calibration finished *\n"
       << "-----------------------------------";
    msg()(sx,"CalibModule::beginTDOCalibration"); sx.str("");
    emit endCalibrationRun();

    emit calibrationLoopState(false);

    return;

}
