/*
 *     #                   #     ###   #  ##
 *     #                   #    #   #  # #
 *   ### # ## #  # # ## ## ###  #  ##   #
 *  #  # ##   #  # ## ## # #  # # ###  # #
 *  #### #     ### #  #  # ####  ### ##  # 808
 *  ---    c++ simple drummachine demo     ---
 * (c) TiM, 2014
 */

#include "soundfile.h"
#include <stdlib.h>
#include <iostream>
#include "RtAudio.h"
#include <cstdlib>
#include <curses.h>

typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE  32767.0

#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )

#define BASE_RATE 0.005
#define TIME   1.0

const char* f_bd = "samples/bd.wav";
const char* f_cb = "samples/cb.wav";
const char* f_ch = "samples/ch.wav";
const char* f_cl = "samples/cl.wav";
const char* f_cp = "samples/cp.wav";
const char* f_cy = "samples/cy.wav";
const char* f_hc = "samples/hc.wav";
const char* f_ht = "samples/ht.wav";
const char* f_lc = "samples/lc.wav";
const char* f_lt = "samples/lt.wav";
const char* f_ma = "samples/ma.wav";
const char* f_mc = "samples/mc.wav";
const char* f_mt = "samples/mt.wav";
const char* f_oh = "samples/oh.wav";
const char* f_rs = "samples/rs.wav";
const char* f_sd = "samples/sd.wav";

class Sampler {
protected:
    bool                trig = false;
    vector<double>*     sample;
    bool                play = false;
    size_t              pointer = 0;
public:
    Sampler()
    {
    }
    Sampler(const char* filename)
    {
        SoundFileRead sample(filename);
        vector<double>* buf = new vector<double>;
        buf->reserve(sample.getSamples()-1);

        for (int i=0; i<sample.getSamples(); i++) 
        {
            buf->insert(buf->end(),sample.getCurrentSampleDouble(0));
            sample.incrementSample();
        }
        this->sample = buf;
    }
    void do_trig()
    {
        this->trig = true;
    }
    double getsample()
    {
        double out;
        if (this->trig)
        {
            this->trig = false;
            this->play = true;
            this->pointer = 0;
        }
        if (this->play)
        {
            out = this->sample->at(this->pointer);
            this->pointer++;
            if (this->pointer > (this->sample->size()-1))
            {
                this->pointer = 0;
                this->play = false;
            }
        }
        return out;
    }
    ~Sampler()
    {
        delete this->sample;
    }
};

vector<double>* load_sample(const char* filename)
{
    SoundFileRead sample(filename);
    vector<double>* buf = new vector<double>;
    buf->reserve(sample.getSamples()-1);

    for (int i=0; i<sample.getSamples(); i++) 
    {
        buf->insert(buf->end(),sample.getCurrentSampleDouble(0));
        sample.incrementSample();
    }
    return buf;
}

RtAudio::StreamOptions options;
unsigned int frameCounter = 0;
bool checkCount = false;
unsigned int nFrames = 0;
const unsigned int callbackReturnValue = 1;
unsigned int channels;
/*
vector<double>* s_bd;
bool x_bd = false;
bool t_bd = false; //trig
unsigned int p_bd = 0; //pointer
*/
Sampler* s_bd;
Sampler* s_cb;
Sampler* s_ch;
Sampler* s_cl;
Sampler* s_cp;
Sampler* s_cy;
Sampler* s_hc;
Sampler* s_ht;
Sampler* s_lc;
Sampler* s_lt;
Sampler* s_ma;
Sampler* s_mc;
Sampler* s_mt;
Sampler* s_oh;
Sampler* s_rs;
Sampler* s_sd;

void errorCallback( RtAudioError::Type type, const std::string &errorText )
{
  // This example error handling function does exactly the same thing
  // as the embedded RtAudio::error() function.
  std::cout << "in errorCallback" << std::endl;
  if ( type == RtAudioError::WARNING )
    std::cerr << '\n' << errorText << "\n\n";
  else if ( type != RtAudioError::WARNING )
    throw( RtAudioError( errorText, type ) );
}

int sampler( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
         double /*streamTime*/, RtAudioStreamStatus status, void *data )
{
    unsigned int i, j;
    extern unsigned int channels;
    MY_TYPE *buffer = (MY_TYPE *) outputBuffer;

    if ( status )
        std::cout << "Stream underflow detected!" << std::endl;
/*
  double increment;
  for ( j=0; j<channels; j++ ) {
    increment = BASE_RATE * (j+1+(j*0.1));
    for ( i=0; i<nBufferFrames; i++ ) {
      *buffer++ = (MY_TYPE) (lastValues[j] * SCALE * 0.5);
      lastValues[j] += increment;
      if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
    }
  }
*/
    double s;
    for (i=0; i<nBufferFrames; i++)
    {
/*     
        if (t_bd)
        {
            t_bd = false;
            x_bd = true;
            p_bd = 0;
        }
        if (x_bd)
        {
            //*buffer++ = (MY_TYPE) (s_bd->at(p_bd) * SCALE);
            s = s_bd->at(p_bd);
            p_bd++;
            if (p_bd > s_bd->size()-1)
            {
                p_bd = 0;
                x_bd = false;
            }
        } else
        {
        //    *buffer++ = (MY_TYPE) (0 * SCALE);
            s = 0;
        }
*/
        s = s_bd->getsample() + 
            s_cb->getsample() + 
            s_ch->getsample() + 
            s_cl->getsample() + 
            s_cp->getsample() + 
            s_cy->getsample() + 
            s_hc->getsample() + 
            s_ht->getsample() + 
            s_lc->getsample() + 
            s_lt->getsample() + 
            s_ma->getsample() + 
            s_mc->getsample() + 
            s_mt->getsample() + 
            s_oh->getsample() + 
            s_rs->getsample() + 
            s_sd->getsample();

        for (j=0; j<channels; j++)
        {
            *buffer++ = (MY_TYPE) (s * SCALE * 0.5);
        }
    }
    frameCounter += nBufferFrames;
    if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
    return 0;
}


int main(int argc, char** argv) 
{
    initscr();
    cbreak();
    printw("drumbox808 v0.01\n");
    // загружаем сэмплы
    //s_bd = load_sample(f_bd);
    s_bd = new Sampler(f_bd);
    s_cb = new Sampler(f_cb);
    s_ch = new Sampler(f_ch);
    s_cl = new Sampler(f_cl);
    s_cp = new Sampler(f_cp);
    s_cy = new Sampler(f_cy);
    s_hc = new Sampler(f_hc);
    s_ht = new Sampler(f_ht);
    s_lc = new Sampler(f_lc);
    s_lt = new Sampler(f_lt);
    s_ma = new Sampler(f_ma);
    s_mc = new Sampler(f_mc);
    s_mt = new Sampler(f_mt);
    s_oh = new Sampler(f_oh);
    s_rs = new Sampler(f_rs);
    s_sd = new Sampler(f_sd);
    // проверяем, есть ли аудиоустройства в системе
    RtAudio dac;
    if ( dac.getDeviceCount() < 1 ) {
        printw("\nNo audio devices found!\n");
        exit( 0 );
    }
    // инициализируем аудио
    // параметры
    unsigned int bufferFrames = 256;
    unsigned int device = 0;
    channels = 2;
    unsigned int offset = 0;
    unsigned int fs = 44100;
    // буфер
//    double *data = (double *) calloc( channels, sizeof( double ) );
    double data[2];
    // Let RtAudio print messages to stderr.
    dac.showWarnings( true );
    // Set our stream parameters for output only.
    RtAudio::StreamParameters oParams;
    oParams.deviceId = device;
    oParams.nChannels = channels;
    oParams.firstChannel = offset;

    if ( device == 0 )
        oParams.deviceId = dac.getDefaultOutputDevice();
    options.flags = RTAUDIO_HOG_DEVICE;
    options.flags |= RTAUDIO_SCHEDULE_REALTIME;
//    options.flags |= RTAUDIO_NONINTERLEAVED;

    // запуск воспроизведения
    try {
        dac.openStream( &oParams, NULL, FORMAT, fs, &bufferFrames, &sampler, (void *)data, &options, &errorCallback );
        dac.startStream();
    }
    catch ( RtAudioError& e ) {
        e.printMessage();
        goto cleanup;
    }

    // ожидание
    if ( checkCount ) {
        while ( dac.isStreamRunning() == true ) SLEEP( 100 );
    }
    else 
    {
        char input;
        //std::cout << "Stream latency = " << dac.getStreamLatency() << "\n" << std::endl;
        printw ("\nPlaying ... enter 'q' to quit (buffer size = %d).\n",bufferFrames);
        printw ("Use keys: z x c v b n m a s d f g h j k l\n");
        while ( input != 'q' )
        {
            input = getch();
//            if (input == 'b') t_bd=true;            
            if (input == 'z') s_bd->do_trig();
            if (input == 'x') s_cb->do_trig();
            if (input == 'c') s_ch->do_trig();
            if (input == 'v') s_cl->do_trig();
            if (input == 'b') s_cp->do_trig();
            if (input == 'n') s_cy->do_trig();
            if (input == 'm') s_hc->do_trig();
            if (input == 'a') s_ht->do_trig();
            if (input == 's') s_lc->do_trig();
            if (input == 'd') s_lt->do_trig();
            if (input == 'f') s_ma->do_trig();
            if (input == 'g') s_mc->do_trig();
            if (input == 'h') s_mt->do_trig();
            if (input == 'j') s_oh->do_trig();
            if (input == 'k') s_rs->do_trig();
            if (input == 'l') s_sd->do_trig();
        }
        try {
            // Stop the stream
            dac.stopStream();
        }
        catch ( RtAudioError& e ) {
            e.printMessage();
        }
    }

cleanup:
    if ( dac.isStreamOpen() ) dac.closeStream();
    endwin();
    return 0;
}
