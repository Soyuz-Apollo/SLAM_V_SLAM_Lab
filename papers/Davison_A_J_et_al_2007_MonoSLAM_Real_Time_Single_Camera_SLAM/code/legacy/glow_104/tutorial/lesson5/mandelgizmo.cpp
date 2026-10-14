/*
===============================================================================

	FILE:  mandelgizmo.cpp
	
	PROJECT:
	
		GLOW-based mandelbrot viewer
	
	CONTENTS:
	
		Gizmo window
	
	PROGRAMMERS:
	
		Daniel Azuma <dazuma@kagi.com>
	
	COPYRIGHT:
	
		Copyright (C) 2000  Daniel Azuma  (dazuma@kagi.com)
		
		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.
		
		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
		GNU Lesser General Public License for more details.
		
		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to
			Free Software Foundation, Inc.
			59 Temple Place, Suite 330
			Boston, MA 02111-1307 USA
	
	VERSION:
	
		The GLOW Toolkit tutorial -- version 1.0.3  (10 September 2000)
	
	CHANGES:
	
		Original release (DA)

===============================================================================
*/


/*
===============================================================================
	Headers
===============================================================================
*/

#include <cstdio>
#include <algorithm>

using namespace std;

#include "glowViewTransform.h"

GLOW_NAMESPACE_USING

#include "mandelgizmo.h"


/*
===============================================================================
	Gizmo drawing component class
===============================================================================
*/

class MandelGizmoComponent :
	public GlowComponent
{
	public:
	
		MandelGizmoComponent(
			GlowComponent* parent,
			const unsigned char* image,
			int width,
			int height);
		~MandelGizmoComponent();
	
	protected:
	
		virtual void OnEndPaint();
	
	private:
	
		GLuint _texObject;
};


/*
===============================================================================
	Gizmo subwindow class
===============================================================================
*/

class MandelGizmoSubwindow :
	public GlowSubwindow
{
	public:
	
		MandelGizmoSubwindow(
			GlowComponent* parent,
			int size,
			int ypos,
			const unsigned char* image,
			int width,
			int height,
			GlowLabelWidget* fpsIndicator);
	
	protected:
	
		virtual bool OnBeginPaint();
		virtual void OnEndPaint();
		
		virtual void OnMouseDown(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
		virtual void OnMouseUp(
			Glow::MouseButton button,
			int x,
			int y,
			Glow::Modifiers modifiers);
		virtual void OnMouseDrag(
			int x,
			int y);
	
	private:
	
		GlowViewManipulator* _manip;
		GlowLabelWidget* _fpsIndicator;
		int _starting;
};


/*
===============================================================================
	Methods of MandelGizmoComponent
===============================================================================
*/

// Constructor

MandelGizmoComponent::MandelGizmoComponent(
	GlowComponent* parent,
	const unsigned char* image,
	int width,
	int height) :
GlowComponent(parent)
{
	GLOW_DEBUGSCOPE("MandelGizmoComponent::MandelGizmoComponent");
	
	// Find texture size because it needs to be a power of 2.
	int texwidth = 2;
	int texheight = 2;
	GLint maxtexsize;
	::glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexsize);
	while (texwidth < width && texwidth < maxtexsize)
	{
		texwidth *= 2;
	}
	while (texheight < height && texheight < maxtexsize)
	{
		texheight *= 2;
	}
	
	// Scale texture image
	unsigned char* teximage = new unsigned char[texwidth*texheight*4];
	::gluScaleImage(GL_RGBA, width, height, GL_UNSIGNED_BYTE, image,
		texwidth, texheight, GL_UNSIGNED_BYTE, teximage);
	
	// Set up texture
	::glGenTextures(1, &_texObject);
	::glBindTexture(GL_TEXTURE_2D, _texObject);
	::glTexImage2D(GL_TEXTURE_2D, 0, 4, texwidth, texheight, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, teximage);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	// Done with temporary storage
	delete[] teximage;
	
	// Make sure we have a sane texture environment
	::glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}


// Destructor

MandelGizmoComponent::~MandelGizmoComponent()
{
	GLOW_DEBUGSCOPE("MandelGizmoComponent::~MandelGizmoComponent");
	// Done with texture object
	::glDeleteTextures(1, &_texObject);
}


// Redraw method

void MandelGizmoComponent::OnEndPaint()
{
	GLOW_DEBUGSCOPE("MandelGizmoComponent::OnEndPaint");
	
	// Material
	static GLfloat diffuse[4]={0.5, 0.5, 0.5, 1.0f};
	static GLfloat ambient[4]={0.4, 0.4, 0.4, 1.0f};
	static GLfloat specular[4]={0.3, 0.3, 0.3, 1.0f};
	static GLfloat emit[4]={0.0, 0.0, 0.0, 1.0f};
	static GLfloat shininess=2.0;
	
	::glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emit);
	::glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	
	::glEnable(GL_TEXTURE_2D);
	::glBegin(GL_QUADS);
	
	// front
	::glNormal3f(0.0f, 0.0f, 1.0f);
	::glTexCoord2f(1.0f, 1.0f);
	::glVertex3f(1.0f, 1.0f, 1.0f);
	::glTexCoord2f(0.0f, 1.0f);
	::glVertex3f(-1.0f, 1.0f, 1.0f);
	::glTexCoord2f(0.0f, 0.0f);
	::glVertex3f(-1.0f, -1.0f, 1.0f);
	::glTexCoord2f(1.0f, 0.0f);
	::glVertex3f(1.0f, -1.0f, 1.0f);
	
	// back
	::glNormal3f(0.0f, 0.0f, -1.0f);
	::glTexCoord2f(0.0f, 1.0f);
	::glVertex3f(1.0f, 1.0f, -1.0f);
	::glTexCoord2f(0.0f, 0.0f);
	::glVertex3f(1.0f, -1.0f, -1.0f);
	::glTexCoord2f(1.0f, 0.0f);
	::glVertex3f(-1.0f, -1.0f, -1.0f);
	::glTexCoord2f(1.0f, 1.0f);
	::glVertex3f(-1.0f, 1.0f, -1.0f);
	
	// right
	::glNormal3f(1.0f, 0.0f, 0.0f);
	::glTexCoord2f(0.0f, 1.0f);
	::glVertex3f(1.0f, 1.0f, 1.0f);
	::glTexCoord2f(0.0f, 0.0f);
	::glVertex3f(1.0f, -1.0f, 1.0f);
	::glTexCoord2f(1.0f, 0.0f);
	::glVertex3f(1.0f, -1.0f, -1.0f);
	::glTexCoord2f(1.0f, 1.0f);
	::glVertex3f(1.0f, 1.0f, -1.0f);
	
	// left
	::glNormal3f(-1.0f, 0.0f, 0.0f);
	::glTexCoord2f(1.0f, 1.0f);
	::glVertex3f(-1.0f, 1.0f, 1.0f);
	::glTexCoord2f(0.0f, 1.0f);
	::glVertex3f(-1.0f, 1.0f, -1.0f);
	::glTexCoord2f(0.0f, 0.0f);
	::glVertex3f(-1.0f, -1.0f, -1.0f);
	::glTexCoord2f(1.0f, 0.0f);
	::glVertex3f(-1.0f, -1.0f, 1.0f);
	
	// top
	::glNormal3f(0.0f, 1.0f, 0.0f);
	::glTexCoord2f(1.0f, 0.0f);
	::glVertex3f(1.0f, 1.0f, 1.0f);
	::glTexCoord2f(1.0f, 1.0f);
	::glVertex3f(1.0f, 1.0f, -1.0f);
	::glTexCoord2f(0.0f, 1.0f);
	::glVertex3f(-1.0f, 1.0f, -1.0f);
	::glTexCoord2f(0.0f, 0.0f);
	::glVertex3f(-1.0f, 1.0f, 1.0f);
	
	// bottom
	::glNormal3f(0.0f, -1.0f, 0.0f);
	::glTexCoord2f(1.0f, 1.0f);
	::glVertex3f(1.0f, -1.0f, 1.0f);
	::glTexCoord2f(0.0f, 1.0f);
	::glVertex3f(-1.0f, -1.0f, 1.0f);
	::glTexCoord2f(0.0f, 0.0f);
	::glVertex3f(-1.0f, -1.0f, -1.0f);
	::glTexCoord2f(1.0f, 0.0f);
	::glVertex3f(1.0f, -1.0f, -1.0f);
	
	::glEnd();
	::glDisable(GL_TEXTURE_2D);
}


/*
===============================================================================
	Methods of MandelGizmoSubwindow
===============================================================================
*/

// Constructor

MandelGizmoSubwindow::MandelGizmoSubwindow(
	GlowComponent* parent,
	int size,
	int ypos,
	const unsigned char* image,
	int width,
	int height,
	GlowLabelWidget* fpsIndicator) :
// Intialize subwindow
GlowSubwindow(parent, 0, ypos, size, size, Glow::rgbaBuffer | Glow::depthBuffer |
	Glow::doubleBuffer, Glow::mouseEvents | Glow::dragEvents)
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::MandelGizmoSubwindow");
	
	// Don't automatically swap buffers
	SetAutoSwapBuffersEnabled(false);
	
	// Attach maniuplator to subwindow
	_manip = new GlowViewManipulator(this,
		GlowViewManipulatorParams::defaults);
	_manip->SetSpinnable(true);
	
	// Attach drawing component to manipulator
	new MandelGizmoComponent(_manip, image, width, height);
	
	// Store away this pointer
	_fpsIndicator = fpsIndicator;
}


// Begin-redraw method

bool MandelGizmoSubwindow::OnBeginPaint()
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnBeginPaint");
	
	// Begin computing frame rate.
	_starting = Glow::GetMilliseconds();
	
	// Clear buffer
	::glClearColor(0.0, 0.0, 0.0, 0.0);
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Set up transforms
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	::glFrustum(-0.5, 0.5, -0.5, 0.5, 1.0, 8.0);
	::glMatrixMode(GL_MODELVIEW);
	
	// Set up lighting
	static GLfloat modelAmb[4] = {0.5f, 0.5f, 0.5f, 1.0f};
	static GLfloat light_amb[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	static GLfloat light_dif[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat light_spe[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	static GLfloat light_pos0[4] = {0.0f, 0.0f, 1.0f, 0.0f};
	
	::glEnable(GL_LIGHTING);
	::glDisable(GL_NORMALIZE);
	::glEnable(GL_DEPTH_TEST);
	::glEnable(GL_CULL_FACE);
	
	::glLightModelfv(GL_LIGHT_MODEL_AMBIENT, modelAmb);
	::glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, (float)GL_TRUE);
	::glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, (float)GL_TRUE);
	
	::glEnable(GL_LIGHT0);
	::glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	::glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
	::glLightfv(GL_LIGHT0, GL_SPECULAR, light_spe);
	::glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	::glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
	::glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
	::glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
	
	::glDisable(GL_LIGHT1);
	::glDisable(GL_LIGHT2);
	::glDisable(GL_LIGHT3);
	
	// Offset camera
	::glTranslatef(0.0f, 0.0f, -4.0f);
	
	return true;
}


// End-redraw method

void MandelGizmoSubwindow::OnEndPaint()
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnEndPaint");
	
	// Flush the pipeline
	::glFlush();
	Glow::SwapBuffers();
	
	// Finish computing frame rate
	char buf[100];
	sprintf(buf, "%.2f fps",
		float(1000)/float(Glow::GetMilliseconds()-_starting));
	_fpsIndicator->SetText(buf);
}


// Respond to mouse down events

void MandelGizmoSubwindow::OnMouseDown(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnMouseDown");
	
	// If the manipulator is not yet active, then begin rotating
	if (_manip->GetState() == GlowViewManipulator::idleState)
	{
		// Note that we need to transform window coordinates to
		// OpenGL coordinates.
		GLfloat xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->BeginRotationDrag(xn, yn);
	}
}


// Respond to mouse up events

void MandelGizmoSubwindow::OnMouseUp(
	Glow::MouseButton button,
	int x,
	int y,
	Glow::Modifiers modifiers)
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnMouseUp");
	
	// If the manipulator is currently rotating, then finish rotating
	if (_manip->GetState() == GlowViewManipulator::rotatingState)
	{
		GLfloat xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->EndDrag(xn, yn);
	}
}


// Respond to mouse drag events

void MandelGizmoSubwindow::OnMouseDrag(
	int x,
	int y)
{
	GLOW_DEBUGSCOPE("MandelGizmoSubwindow::OnMouseDrag");
	
	// If the manipulator is currently rotating, then pass drag event
	// on to the manipulator
	if (_manip->GetState() == GlowViewManipulator::rotatingState)
	{
		GLfloat xn, yn;
		NormalizeCoordinates(x, y, xn, yn);
		_manip->InDrag(xn, yn);
	}
}


/*
===============================================================================
	Methods of MandelGizmoWindow
===============================================================================
*/

// Constructor

MandelGizmoWindow::MandelGizmoWindow(
	const unsigned char* image,
	int width,
	int height)
{
	GLOW_DEBUGSCOPE("MandelGizmoWindow::MandelGizmoWindow");
	
	// Initialize window
	Init("MandelGizmo", GlowWindow::autoPosition, GlowWindow::autoPosition,
		200, 200, Glow::rgbBuffer, Glow::noEvents);
	
	// Create widget panel with close button and frame rate indicator.
	// Note that we use a margin of 0
	_widgetsPanel = new GlowQuickPaletteSubwindow(this, 0, 0, 200, 25,
		GlowQuickPalette::horizontal, GlowQuickPalette::alignCenter, 10, 0, 0);
	_widgetsPanel->AddDismissPushButton("Close", this);
	// Note that we provide initial text for the label widget so its
	// computed width is wide enough to display any fps we want.
	GlowLabelWidget* fpsIndicator = _widgetsPanel->AddLabel("0000.00 fps");
	_widgetsPanel->Pack(true);
	
	// Create the subwindow that will contain the cube drawing
	_gizmoPanel = new MandelGizmoSubwindow(this, 200, _widgetsPanel->Height(),
		image, width, height, fpsIndicator);
	Reshape(200, 200+_widgetsPanel->Height());
}


// Respond to reshape events

void MandelGizmoWindow::OnReshape(
	int width,
	int height)
{
	GLOW_DEBUGSCOPE("MandelGizmoWindow::OnReshape");
	
	// Enforce minimum size
	if (width < 40 || height < 40+_widgetsPanel->Height())
	{
		Reshape(max(width, 40), max(height, 40+_widgetsPanel->Height()));
	}
	else
	{
		// Reshape the widget panel if the width has changed
		if (width != _widgetsPanel->Width())
		{
			_widgetsPanel->Reshape(width, _widgetsPanel->Height());
		}
		
		// Position and size the drawing subwindow.
		height -= _widgetsPanel->Height();
		int gizmoSize = min(width, height);
		_gizmoPanel->Move((width-gizmoSize)/2,
			_widgetsPanel->Height()+(height-gizmoSize)/2);
		_gizmoPanel->Reshape(gizmoSize, gizmoSize);
	}
	
	// Update the viewport
	::glViewport(0, 0, width, height);
}

