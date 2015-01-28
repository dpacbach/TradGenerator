// Irish Tune Generator

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

using namespace std;

int *Initialize_Tune(void);
void Generate_Tune(int *tune);
void Create_Melody(int *melody, int level, int goal_note);
void Iterate_Melody(int *tune, int location, int level);
void Output_Level(int *tune, int level, ostream &out);
void Output_Tune(int *tune);
char Note_To_Char(int);
void Set_Block_Note(int *tune, int location, int level, int note);
void Apply_Constraints(int *tune, int level);
int  Gaussian(int level, float spread, float offset);
void Initialize_RNG(void);
void Initialize_Tune_Data(void);

int l_size[7] = {1, 2, 2, 2, 2, 2, 3};
int level_speeds[7];
int tune_size = 1;
int num_levels;
float level_spreads[7];

//===== START FREE PARAMETERS ==================================================

const float SPREAD_COEFF  = .6;
const float SPREAD_EXP    =  1.5;
const float SPREAD_OFFSET =  (1-SPREAD_COEFF);

void Create_Melody(int *melody, int level, int goal_note)
{
	int first_note = melody[0];
	int prev_note = first_note;
	
	for (int note_index = 1; note_index < l_size[level]; note_index++)
	{
		int note = -1, tritone = 0;
		while (note < 0 || note > 12 || tritone == 1)
		{
			float spread = level_spreads[level];
			float offset = (float)(goal_note-prev_note)*((float)note_index/(float)l_size[level]);

			note = prev_note + Gaussian(level, spread, offset);
			
			if ((note == 3 && prev_note == 6) || (note == 6 && prev_note == 3) || (note == 10 && prev_note == 6) || (note == 6 && prev_note == 10))
				tritone = 1;
			else
				tritone = 0;
		}
		
		melody[note_index] = note;
		prev_note = note;
	}
}

void Apply_Constraints(int *tune, int level)
{
	for (int i = 0; i < 12; i++)
		tune[24 + i] = tune[i];
	for (int i = 0; i < 12; i++)
		tune[84 + i] = tune[36 + i];
}

//===== END FREE PARAMETERS ==================================================

int main(void)
{
	int *tune;

	Initialize_RNG();
	Initialize_Tune_Data();
	tune = Initialize_Tune();
	Iterate_Melody(tune, 0, 0);
	Output_Tune(tune);

	delete tune;
	//cout << endl << endl;
	
	return 0;
}

void Iterate_Melody(int *tune, int location, int level)
{
	int sub_size = 1;
	for (int j = level+1; j < num_levels; j++)
		sub_size *= l_size[j];

	int goal_note = tune[(location + sub_size*l_size[level]) % tune_size];
	int *melody = new int[l_size[level]];
	melody[0] = tune[location];
	
	Create_Melody(melody, level, goal_note);

	for (int i = 0; i < l_size[level]; i++)
		Set_Block_Note(tune, location + sub_size*i, level, melody[i]);

	delete melody;
	
	Apply_Constraints(tune, level);

	if (sub_size > 1)
	{
		for (int section = 0; section < l_size[level]; section++)
			Iterate_Melody(tune, location + section*sub_size, level+1);
	}
}

int *Initialize_Tune(void)
{
	int *temp_tune = new int[tune_size], note;

	do
	{
		note = rand() % 9;
		Set_Block_Note(temp_tune, 0, 0, note);
	}	while (note == 6);

	return temp_tune;
}

int Gaussian(int level, float spread, float offset)
{
	float area = spread*sqrt(3.14159), sum = 0, result;
	float random_number = (float)(rand() % 1000)/1000.0;
	
	for (result = offset-(spread*10.0); result < offset+(spread*10.0); result += .01)
	{
		sum += .01*exp(-pow((result-offset)/spread, 2.0))/area;
		if (sum > (float)random_number)
			break;
	}

	if (result > 0)
		return (int)(result+.5);
	else
		return (int)(result-.5);
}

void Output_Tune(int *tune)
{
	ofstream out("tune.abc");

	for (int k = 0; k < num_levels; k++)
		Output_Level(tune, k, out);

	//Output_Level(tune, num_levels-1, cout);

	out.close();
}

void Output_Level(int *tune, int level, ostream &out)
{
	out << endl << endl;
	out << endl << "X:" << num_levels-level << endl << "T:Jig" << endl << "M:6/8" << endl << "L:1/8"
		 << endl << "Q:1/8=" << level_speeds[level] << endl << "R:jig" << endl << "K:D";

	int sub_size = 1;
	for (int j = level+1; j < num_levels; j++)
		sub_size *= l_size[j];

	int group_size = 2;
	if (l_size[level] % 2 > 0)
		group_size = 3;
		
	for (int i = 0; i < tune_size/sub_size; i++)
	{
		
		if (i % (8*group_size) == 0)
			out << endl;

		if (i % (2*group_size) == 0)
			out << " ";

		if (i % group_size == 0 && tune[i*sub_size] == tune[(i+1)*sub_size])
		{
			if (group_size == 3)
			{
				if (tune[i*sub_size] == tune[(i+2)*sub_size])
				{
					out << "" << Note_To_Char(tune[i*sub_size]) << "3";
					//out << "~" << Note_To_Char(tune[i*sub_size]) << "3";
					i += 2;
				}
				else
				{
					out << Note_To_Char(tune[i*sub_size]) << "2";
					i += 1;
				}
			}
			else
			{
				out << Note_To_Char(tune[i*sub_size]) << Note_To_Char(tune[i*sub_size]);//"2";
				i += 1;
			}
		}
		else
			out << Note_To_Char(tune[i*sub_size]);
		
		if ((i+1) % group_size == 0)
			out << " ";
		if ((i+1) % (2*group_size) == 0)
			out << "|";
	}
	if (tune_size/sub_size % (2*group_size) > 0)
		out << "|";

	out << "]";
}

char Note_To_Char(int note)
{
	char notes[] = "DEFGABcdefgab";
	return notes[note];
}

void Set_Block_Note(int *tune, int location, int level, int note)
{
	int sub_size = 1;
	for (int j = level+1; j < num_levels; j++)
		sub_size *= l_size[j];

	for (int i = 0; i < sub_size; i++)
		tune[location + i] = note;
}

void Initialize_RNG(void)
{
	unsigned long int key = time(NULL);
	srand(key);
	//cout << endl << "          Key = " << key;
}

void Initialize_Tune_Data(void)
{
	int j;
	num_levels = sizeof(l_size)/sizeof(int);
	
	for (j = 0; j < num_levels; j++)
		tune_size *= l_size[j];

	int spread = 1;
	//cout << endl << "Level Spreads = {";
	for (j = num_levels; j > 0; j--)
	{
		level_spreads[j-1] = SPREAD_COEFF*pow(spread, SPREAD_EXP) + SPREAD_OFFSET;
		spread *= l_size[j-1];
		//cout << level_spreads[j-1];
		//if (j > 1)
			//cout << ", ";
	}
	//cout << "}";
	int speed = 300;
	for (j = num_levels; j > 0; j--)
	{
		level_speeds[j-1] = speed;
		speed /= l_size[j-1];
	}
}

