#ifndef COMPONENT_BANNERS_H
#define COMPONENT_BANNERS_H

#include <GL/glut.h>
#include <string>
#include <cstring>
#include "parameter.h"

// Helper to draw text
inline void drawBitmapText(const char* string, float x, float y, float z) {
    const char* c;
    glRasterPos3f(x, y, z);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

inline void drawSmallText(const char* string, float x, float y, float z) {
    const char* c;
    glRasterPos3f(x, y, z);
    float startX = x;
    float currentY = y;
    for (c = string; *c != '\0'; c++) {
        if (*c == '\n') {
            currentY -= 0.15f; // Move down
            glRasterPos3f(startX, currentY, z); // Reset X
        } else {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }
    }
}

inline void drawBannerBackground(float x, float y, float z, float width, float height) {
    // Semi-transparent background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw quad
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f); // Dark translucent
    glVertex3f(x, y, z);
    glVertex3f(x + width, y, z);
    glVertex3f(x + width, y - height, z);
    glVertex3f(x, y - height, z);
    glEnd();
    
    // Draw border
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glColor4f(0.0f, 1.0f, 1.0f, 1.0f); // Cyan border
    glVertex3f(x, y, z);
    glVertex3f(x + width, y, z);
    glVertex3f(x + width, y - height, z);
    glVertex3f(x, y - height, z);
    glEnd();

    glDisable(GL_BLEND);
}

// Draw a nicer banner that tries to face the user (simple billboarding or just fixed offset)
// Since this is a simple project, we'll just draw it at a fixed offset relative to component.
inline void renderBanner(float px, float py, float pz, const char* title, const char* desc) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    
    // Position near the component
    glTranslatef(px, py, pz);
    
    // FIX: Removed 90 degree rotation to ensure the banner faces the Z-axis (camera)
    // Small tilt to look nicer
    glRotatef(5, 1, 0, 0); 

    float w = 5.5f; // Slightly wider for text
    float h = 1.6f; // Slightly taller
    
    drawBannerBackground(0, 0, 0, w, h);
    
    glColor3f(1.0f, 1.0f, 0.0f); // Title in Yellow
    drawBitmapText(title, 0.2f, -0.4f, 0.01f);
    
    glColor3f(1.0f, 1.0f, 1.0f); // Desc in White
    drawSmallText(desc, 0.2f, -0.8f, 0.01f);
    
    glPopMatrix();
    glEnable(GL_TEXTURE_2D); // Restore
    glEnable(GL_LIGHTING);
}

inline void drawComponentInfo(int currentObjIndex) {
    // Coordinates tuned to appear near components but visible to camera (Z ~ -2 to -4)
    // Camera is at Z=5 looking at -Z. 
    
    if (currentObjIndex == REMOVE_SIDE_PANEL) {
        renderBanner(2.0f, 4.0f, 0.0f, "SIDE PANEL", "Protects internal components\nfrom dust and damage.");
    }
    else if (currentObjIndex == REMOVE_RAM_STICK) {
         renderBanner(3.0f, 6.0f, -2.0f, "RAM (Memory)", "Random Access Memory.\nStores temporary data for CPU.");
    }
    else if (currentObjIndex == REMOVE_FAN) {
         renderBanner(1.0f, 5.0f, -1.0f, "COOLING FAN", "Dissipates heat from the CPU\nto prevent overheating.");
    }
    else if (currentObjIndex == REMOVE_PROCESSOR) {
         renderBanner(1.0f, 5.0f, -1.0f, "CPU (Processor)", "Central Processing Unit.\nThe brain of the computer.");
    }
    else if (currentObjIndex == REMOVE_PSU) {
         renderBanner(2.0f, 7.0f, 0.0f, "PSU (Power Supply)", "Converts outlet power for\ninternal components.");
    }
    else if (currentObjIndex == REMOVE_HDD) {
         renderBanner(4.0f, 3.0f, 0.0f, "HDD (Storage)", "Hard Disk Drive.\nStores files and OS permanently.");
    }
    else if (currentObjIndex == REMOVE_GPU) {
         renderBanner(2.0f, 3.5f, -1.0f, "GPU (Graphics Card)", "Renders images and video\nfor the display.");
    }
    else if (currentObjIndex == REMOVE_MOTHERBOARD) {
         renderBanner(0.0f, 3.0f, -2.0f, "MOTHERBOARD", "Main circuit board connecting\nall components.");
    }
}

#endif
