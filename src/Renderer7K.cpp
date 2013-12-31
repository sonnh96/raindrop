#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Global.h"
#include "Screen.h"
#include "Audio.h"

#include "GameWindow.h"
#include "Rendering.h"
#include "VBO.h"
#include "GraphObject2D.h"
#include "Image.h"
#include "ImageLoader.h"

#include "Song.h"
#include "ScreenGameplay7K.h"



void ScreenGameplay7K::DrawMeasures()
{
	typedef std::vector<SongInternal::Measure<TrackNote>> NoteVector;
	float rPos = CurrentVertical * SpeedMultiplier + BasePos;

	// Assign our matrix.
	WindowFrame.SetUniform("mvp", &PositionMatrix[0][0]);

	// Set the color.
	WindowFrame.SetUniform("Color", 1, 1, 1, 1);
	WindowFrame.SetUniform("inverted", false);
	WindowFrame.SetUniform("AffectedByLightning", false);
	WindowFrame.SetUniform("useTranslate", true);
	WindowFrame.SetUniform("Centered", true);
	WindowFrame.SetUniform("sMult", SpeedMultiplier);
	GraphObject2D::BindTopLeftVBO();

	glVertexAttribPointer( WindowFrame.EnableAttribArray("position"), 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0 );

	/* todo: instancing */
	for (uint32 k = 0; k < Channels; k++)
	{
		NoteVector &Measures = NotesByMeasure[k];

		for (NoteVector::iterator i = Measures.begin(); i != Measures.end(); i++)
		{
			for (std::vector<TrackNote>::iterator m = (*i).MeasureNotes.begin(); m != (*i).MeasureNotes.end(); m++)
			{
				/* This is the last note in this measure. */
				float Vertical = (m->GetVertical()* SpeedMultiplier + rPos) ;
				if (Vertical < 0 || Vertical > ScreenHeight)
					continue; /* If this is not visible, we move on to the next one. */

				if (NoteImages[k])
					NoteImages[k]->Bind();
				else
				{
					if (NoteImage)
						NoteImage->Bind();
					else
						continue;
				}

				WindowFrame.SetUniform("tranM", &(m->GetMatrix())[0][0]);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}
		}
	}

	WindowFrame.DisableAttribArray("position");
}

void ScreenGameplay7K::DrawExplosions()
{
	for (int i = 0; i < CurrentDiff->Channels; i++)
	{
		int Frame = ExplosionTime[i] / 0.016;

		if (Frame > 19)
			Explosion[i].Alpha = 0;
		else
		{
			Explosion[i].Alpha = 1;
			Explosion[i].SetImage ( ExplosionFrames[Frame], false );
		}

		Explosion[i].Render();

	}
}