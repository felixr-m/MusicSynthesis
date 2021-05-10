#pragma once

#include "ofMain.h"
#include "ofxGUi.h"
#include "ofxMaxim.h"
#include "ofxMidi.h"

class ofApp : public ofBaseApp, public ofxMidiListener{

	public:
		void setup();
		void update();
		void draw();

		void exit()override;
		void newMidiMessage(ofxMidiMessage &message)override;

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void audioOut(ofSoundBuffer& output) override;
		void audioIn(ofSoundBuffer& output) override;

		float noteToFreq(int pitch, int instrument);
		


		ofSoundStream soundStream;
		
		//Violin Variables
		ofxMaxiOsc violinCarrier, violinVibrato;
		ofxMaxiFilter violinLP, violinHP, violinFB1, violinFB2, violinFB3;
		ofxMaxiEnvelope violinEnv;
		ofxMaxiDelayline violinChorus;
		double violinCarrierf, violinLPf,  violinHPf, violinFBf, violinFBf1, violinFBf2, violinEnvf, violinChorusf;

		//Flute Variables
		ofxMaxiOsc fluteOsc, fluteWhistle, FluteLowPassOsc, FluteLowPass2Osc;
		ofxMaxiFilter fluteLP, fluteLP2, fluteHP, fluteWhistleHP, fluteWhistleLP;
		ofxMaxiEnvelope fluteEnv;
		ofxMaxiDelayline fluteChorus;
		double fluteOscf, fluteLPf, fluteLP2f, fluteHPf, fluteWhistlef, fluteEnvf, fluteChrousf, fluteWhistleHPf, fluteWhistleLPf;

		//Trumpet Variables
		ofxMaxiOsc trumpetOsc, trumpetOsc2, trumpetEnvOsc, trumpetGrowl;
		ofxMaxiFilter trumpetLP, trumpetLP2, trumpetLP3, trumpetHP, trumpetHR, trumpetHR2, trumpetHR3;
		ofxMaxiEnvelope trumpetEnv, trumpetEnv2;
		double trumpetOscf, trumpetOsc2f, trumpetLPf, trumpetLP2f, trumpetLP3f, trumpetHPf, trumpetEnvf, trumpetEnv2f, trumpetHRf, trumpetHR2f, trumpetHR3f, trumpetGrowlf;

		//Real Instruments
		ofxMaxiSample flute, violin, trumpet;

		//Drawing waveform Variables
		float waveform[4096]; //make this bigger, just in case
		int waveIndex;

		//Gui Variables
		ofxPanel gui;
		ofxIntSlider instrumentSlider;
		//ofxFloatSlider fequencySlider;

		//Midi Variables
		ofxMidiIn midi;
		vector <ofxMidiMessage> messages;

		//FFT Variables
		ofxMaxiFFT myFFT, myFFT2;
		std::vector<float> currentFreqs;

		int currentInstrument;

		float frequency = 524;
		int velocity = 1;
		
};
