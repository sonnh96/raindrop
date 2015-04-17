#include "GameGlobal.h"
#include "Song7K.h"
#include "Logging.h"

using namespace VSRG;

static double PassThrough(double D){
	return D;
}

RowifiedDifficulty::RowifiedDifficulty(Difficulty *Source, bool Quantize, bool CalculateAll)
	: Quantizing(Quantize), Parent(Source)
{
	assert(Source != nullptr);

	if (Quantize)
		QuantizeFunction = function<double(double)>(QuantizeBeat, _1);
	else
		QuantizeFunction = function<double(double)>(PassThrough, _1);

	Source->ProcessBPS(BPS, 0);

	CalculateMeasureAccomulation();

	if (CalculateAll)
	{
		CalculateBGMEvents();
		CalculateObjects();
	}
}

bool RowifiedDifficulty::IsQuantized()
{
	return Quantizing;
}

void RowifiedDifficulty::CalculateMeasureAccomulation() 
{
	double Acom = 0;

	assert(Parent != nullptr);

	MeasureAccomulation.clear();
	for (auto M : Parent->Data->Measures) {
		MeasureAccomulation.push_back(QuantizeFunction(Acom));
		Acom += M.MeasureLength;
	}
}

IFraction RowifiedDifficulty::FractionForMeasure(int Measure, double Beat)
{
	double mStart = MeasureAccomulation[Measure];
	double mLen = QuantizeFunction(Parent->Data->Measures[Measure].MeasureLength);
	double mFrac = (Beat - mStart) / mLen;
	LFraction Frac;

	if (!IsQuantized())
		Frac.fromDouble(mFrac);
	else
		Frac.fromDouble(QuantizeFractionMeasure(mFrac));

	return IFraction{ Frac.Num, Frac.Den };
}

int RowifiedDifficulty::MeasureForBeat(double Beat)
{
	int Measure = -1;
	for (auto i : MeasureAccomulation)
	{
		if (Beat >= i)
			Measure += 1;
		else return Measure;
	}

	throw std::runtime_error("Beat beyond last measure.");
}

void RowifiedDifficulty::ResizeMeasures(size_t NewMaxIndex) {
	if (Measures.size() < NewMaxIndex + 1)
		Measures.resize(NewMaxIndex + 1);
}

void RowifiedDifficulty::CalculateBGMEvents()
{
	for (auto BGM : Parent->Data->BGMEvents) {
		double Beat = QuantizeFunction(IntegrateToTime(BPS, BGM.Time));

		int MeasureForEvent = MeasureForBeat(Beat);
		ResizeMeasures(MeasureForEvent);
		Measures[MeasureForEvent].BGMEvents.push_back({ FractionForMeasure(MeasureForEvent, Beat), BGM.Sound });
	}
}

void RowifiedDifficulty::CalculateObjects()
{
	for (auto M : Parent->Data->Measures) {
		for (int K = 0; K < Parent->Channels; K++) {
			for (auto N : M.MeasureNotes[K]) {
				double StartBeat = QuantizeFunction(IntegrateToTime(BPS, N.StartTime));

				if (StartBeat < 0) {
					Log::Printf("Object at negative beat (%f), discarded\n", StartBeat);
					continue;
				}

				if (N.EndTime == 0){ // Non-hold. Emit channels 11-...

					int MeasureForEvent = MeasureForBeat(StartBeat);
					ResizeMeasures(MeasureForEvent);

					int Snd = N.Sound ? N.Sound : 1;
					Measures[MeasureForEvent].Objects[K].push_back({ FractionForMeasure(MeasureForEvent, StartBeat), Snd });
				}
				else { // Hold. Emit channels 51-...
					double EndBeat = QuantizeFunction(IntegrateToTime(BPS, N.EndTime));
					int MeasureForEvent = MeasureForBeat(StartBeat);
					int MeasureForEventEnd = MeasureForBeat(EndBeat);
					ResizeMeasures(MeasureForEventEnd);

					int Snd = N.Sound ? N.Sound : 1;
					Measures[MeasureForEvent].LNObjects[K].push_back({ FractionForMeasure(MeasureForEvent, StartBeat), Snd });
					Measures[MeasureForEventEnd].LNObjects[K].push_back({ FractionForMeasure(MeasureForEventEnd, EndBeat), Snd });
				}
			}
		}
	}
}

double RowifiedDifficulty::GetStartingBPM() {
	return BPS[0].Value * 60;
}