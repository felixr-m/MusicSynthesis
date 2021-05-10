#include "ofApp.h"

//https://www.soundonsound.com/techniques/synthesizing-acoustic-pianos-roland-jx10-1102
//https://pages.mtu.edu/~suits/notefreqs.html
//https://www.soundonsound.com/techniques/practical-bowed-string-synthesis
//http://hyperphysics.phy-astr.gsu.edu/hbase/Music/violin.html
//https://www.youtube.com/watch?v=O16z27C4a7Q&ab_channel=LewisLepton
//https://freesound.org/people/MTG/

//Used wrong formular to calulate notes originally just used frequency = pitch * 10.9166;


//Creating Vector for FFT frequencies 
std::vector<float> fftFreqs(int fftSize, int sampleRate)
{
	std::vector<float> theFreqs;
	for (int i = 0; i < fftSize; ++i)
	{
		theFreqs.push_back((sampleRate / float(fftSize)) * i);
	}
	return theFreqs;
}

//--------------------------------------------------------------
void ofApp::setup(){
	//Setting Background Colour
	ofBackground(4, 30, 66);
	
	//setting up Gui with Slider to control which instrument is being played
	gui.setup("Settings");
	gui.add(instrumentSlider.setup("Instrument", 3, 1, 3));

	//Initialize the drawing variables
	for (int i = 0; i < ofGetWidth()/2; ++i) {
		waveform[i] = 0;
	}
	waveIndex = 0;

	//inititalising variables for the project
	int sampleSize = 44100;
	int channelSize = 2;
	int bufferSize = 512;

	//applying the variables to the maximillian library
	maxiSettings::setup(
		sampleSize,
		channelSize,
		bufferSize
	);

	//creating sound stream settings
	ofSoundStreamSettings settings;

	auto devices = soundStream.getMatchingDevices("default");

	if (!devices.empty()) {
		settings.setInDevice(devices[0]);
	}

	//applying settings
	settings.setInListener(this);
	settings.setOutListener(this);
	settings.sampleRate = sampleSize;
	settings.numInputChannels = channelSize;
	settings.numOutputChannels = channelSize;
	settings.bufferSize = bufferSize;

	soundStream.setup(settings);

	//Midi Stuff
	midi.listInPorts();
	midi.openPort(0);
	midi.addListener(this);

	//FFT stuff
	int fftSize = 1024;
	myFFT.setup(fftSize, fftSize, fftSize / 2);

	currentFreqs = fftFreqs(fftSize, sampleSize);	
}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	//Drawing Gui 
	gui.draw();

	//Text to tell user what instruement sound they are currently using
	ofSetColor(255);
	ofDrawBitmapString("Frequency " + ofToString(frequency), ofGetWidth() - 150, 50);
	switch (instrumentSlider) {
	case 1:
		ofDrawBitmapString("String", ofGetWidth() - 150, 100);
		break;
	case 2:
		ofDrawBitmapString("Woodwind", ofGetWidth() - 150, 100);
		break;
	case 3:
		ofDrawBitmapString("Brass", ofGetWidth() - 150, 100);
		break;
	}

	//Waveform Drawing
	ofTranslate(0, ofGetHeight() / 3);
	ofSetColor(175, 234, 220);
	ofFill();
	ofSetLineWidth(2);
	ofDrawLine(0, 0, 1, waveform[1] * ofGetHeight() / 2.); //first line
	for (int i = 1; i < (ofGetWidth() - 1); ++i)
	{
		ofDrawLine(i, waveform[i] * ofGetHeight()/2, i + 1, waveform[i + 1] * ofGetHeight()/2);
	}

	//FFT Drawing
	ofTranslate(0, ofGetHeight() / 3 * 2 - ofGetHeight() / 6);
	
	int binWidth = ofGetWidth() / myFFT.bins;
	ofDrawLine(0, 0, ofGetWidth(), 0);

	ofSetColor(175, 234, 220);
	for (size_t i = 0; i < myFFT.bins; ++i)
	{
		// one rectangle per bin
		ofDrawRectangle(i*binWidth, 0, binWidth, myFFT.magnitudes[i] * -10.);
	}
	
}

//-------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& output) {
	std::size_t nChannels = output.getNumChannels();
	for (int i = 0; i < output.getNumFrames(); ++i) {

		//Violin - influenced by https://www.soundonsound.com/techniques/practical-bowed-string-synthesis 
		//http://www.audiomisc.co.uk/asymmetry/asym.html - Figure G
		violinCarrierf = violinCarrier.sawn(frequency + violinVibrato.sinewave(8)/2);
		violinLPf = violinLP.lores(violinCarrierf, 300, 0);
		violinHPf = violinHP.hires(violinLPf, 4000, 0);
		violinFBf = violinFB1.hires(violinHPf, 300, 3.5);
		violinFBf1 = violinFB2.hires(violinFBf, 700, 3.5);
		violinFBf2 = violinFB3.hires(violinFBf1, 3000, 2);
		violinEnvf = violinEnv.adsr(0.5, 0.5, 0, 5);
		violinChorusf = violinChorus.dl(violinFBf2, 20, 0.5);
	

		//flute - influenced by https://www.soundonsound.com/techniques/practical-flute-synthesis
		//http://hyperphysics.phy-astr.gsu.edu/hbase/Music/flutew.html
		fluteOscf = fluteOsc.sawn(frequency);
		fluteHPf = fluteHP.hires(fluteOscf, 3000, 0);
		fluteLPf = fluteLP.lores(fluteHPf, 458.5 + FluteLowPassOsc.sinewave(10), 0);
		fluteChrousf = fluteChorus.dl(fluteLPf, 150, 1);
		fluteEnvf = fluteEnv.adsr(0.2, 0.9, 0, 0.5);

		fluteWhistlef = fluteWhistle.sinewave(rand() * 2 - 1)/100;
		fluteWhistleHPf = fluteWhistleHP.hires(fluteWhistlef, 200, 0);
		fluteWhistleLPf = fluteWhistleLP.lores(fluteWhistleHPf, 205, 0);


		//Trumpet - Influenced by https://www.soundonsound.com/techniques/synthesizing-brass-instruments
		//high at 524 then higher at 1048 then exponentially down
		trumpetOscf = trumpetOsc.sawn(frequency);
		trumpetLPf = trumpetLP.lores(trumpetOscf, 2097, 10);
		trumpetHRf = trumpetHR.hires(trumpetLPf, 2095, 10);
		trumpetLP2f = trumpetLP2.lores(trumpetHRf, 1049, 10);
		trumpetHR2f = trumpetHR2.hires(trumpetLP2f, 1047, 10);
		trumpetLP3f = trumpetLP3.lores(trumpetHR2f, 525, 10);
		trumpetHR3f = trumpetHR3.hires(trumpetLP3f, 523, 10);
		trumpetHPf = trumpetHP.bandpass(trumpetHR3f, 0.1, 0.1);
		trumpetEnvf = trumpetEnv.adsr(0.5, 0, 1, 0.1);
		trumpetGrowlf = trumpetGrowl.triangle(160);
		
		//Switch Statement to control which instruemnt sound is played
		switch (instrumentSlider){
		case 1:
			output[i * nChannels] = violinChorusf * velocity * violinEnvf * 10;
			break;
		case 2:
			output[i * nChannels] = fluteLPf * 5 * fluteEnvf + fluteWhistlef * fluteEnvf;
			break;
		case 3:
			output[i * nChannels] = (trumpetHPf * 2 + trumpetGrowlf * 0.25 + fluteWhistlef * 25) * velocity * 0.0005 * trumpetEnvf;
		}
		//output[i * nChannels] = fluteHPf * 10 * fluteEnvf;
		output[1 + i * nChannels] = output[i * nChannels];

		//Applying FFt to current Output Sound
		myFFT.process(output[i * nChannels]);

		//Setting Waveform to current Output Sound
		waveform[waveIndex] = output[i * nChannels];
		++waveIndex;
		if (waveIndex > (ofGetWidth() - 1)) waveIndex = 0;

	}
}

//-------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {
	std::size_t nChannels = input.getNumChannels();
	for (int i = 0; i < input.getNumFrames(); ++i) {
		input[i * nChannels] = 0;
		input[1 + i * nChannels] = input[i * nChannels];
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::exit() {
	//To Close Midi
	midi.closePort();
	midi.removeListener(this);
}

void ofApp::newMidiMessage(ofxMidiMessage &message) {
	int messageNo = 0;
	messageNo++;
	if (messageNo % 2 == 0) {
		messages.push_back(message);
	}
	

	if (messages.size() > 8) {
		messages.erase(messages.begin());
	}

	int pitch = 0;
	pitch = message.pitch;

	velocity = message.velocity;

	frequency = noteToFreq(pitch, instrumentSlider);

	trumpetEnv.trig = 1;
	violinEnv.trig = 1;
	fluteEnv.trig = 1;
}

float ofApp::noteToFreq(int note, int instrument) {
	//https://gist.github.com/YuxiUx/ef84328d95b10d0fcbf537de77b936cd
	float a = 1760;
	if (instrument == 3) {
		a = 440;
	}
	else {
		a = 1760;
	}
	 //frequency of A6 - Soprano Instruments
	return (a / 32) * pow(2, ((note - 9) / 12.0));
}