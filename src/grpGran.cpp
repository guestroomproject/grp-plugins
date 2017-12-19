#include "grp-plugins.hpp"
#include "grp-utils.hpp"

struct GrpGran : Module {
	enum ParamIds {
		GATE_PARAM,
		GRAIN_SIZE_PARAM,
		POSITION_PARAM,
		MIX_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		AUDIO_INPUT,
		GATE_INPUT,
		GRAIN_SIZE_INPUT,
		POSITION_INPUT,
		MIX_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		MIX_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		REC_LIGHT,
		NUM_LIGHTS
	};

	int buffer_size;
	float* recBuffer;
	int start, end, cur;

	void step() override;

	GrpGran();

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - reset, randomize: implements special behavior when user clicks these from the context menu
};

GrpGran::GrpGran() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS),
start(0), end(0), cur(0)
{
		//5 sec circular buffer that is power of 2
		buffer_size = 1 << log2i(5*engineGetSampleRate());
		recBuffer = new float[buffer_size];
}


void GrpGran::step() {

	float currentOUT = 0.0;

	bool rec = (params[GATE_PARAM].value == 1.0) ||
				inputs[GATE_INPUT].value > 0.0;
	
	if(rec){
		recBuffer[start++] = inputs[AUDIO_INPUT].value;
	}

	//outputs
	lights[REC_LIGHT].value = rec ? 10.0 : 0.0;
	outputs[MIX_OUTPUT].value = 
	 inputs[AUDIO_INPUT].value * (1-params[MIX_PARAM].value) + 
	 currentOUT * params[MIX_PARAM].value;
}


GrpGranWidget::GrpGranWidget() {
	GrpGran *module = new GrpGran();
	setModule(module);
	box.size = Vec(12 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/grpGran.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	addInput(createInput<PJ301MPort>(Vec(10, 45), module, GrpGran::GATE_INPUT));
	addParam(createParam<LEDButton>(Vec(60, 45), module, GrpGran::GATE_PARAM, 0.0, 1.0, 0.0));
	addChild(createLight<MediumLight<GreenLight>>(Vec(60+7, 45+2), module, GrpGran::REC_LIGHT));

	addInput(createInput<PJ301MPort>(Vec(10, 100), module, GrpGran::GRAIN_SIZE_INPUT));
	addParam(createParam<Davies1900hBlackKnob>(Vec(50, 80), module, GrpGran::GRAIN_SIZE_PARAM, 0.0, 10.0, 0.0));

	addInput(createInput<PJ301MPort>(Vec(10, 140), module, GrpGran::POSITION_INPUT));
	addParam(createParam<Davies1900hBlackKnob>(Vec(50, 130), module, GrpGran::POSITION_PARAM, 0.0, 10.0, 0.0));

	addInput(createInput<PJ301MPort>(Vec(70, 270), module, GrpGran::MIX_INPUT));
	addParam(createParam<Davies1900hBlackKnob>(Vec(70, 230), module, GrpGran::MIX_PARAM, 0.0, 1.0, 0.5));

	addInput(createInput<PJ301MPort>(Vec(35, box.size.y-65), module, GrpGran::AUDIO_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(107, box.size.y-65), module, GrpGran::MIX_OUTPUT));
}
