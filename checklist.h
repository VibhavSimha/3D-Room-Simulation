#ifndef CHECKLIST_H
#define CHECKLIST_H

#include <GL/glut.h>
#include <vector>
#include <string>
#include "parameter.h" 

struct ChecklistItem {
    std::string text;
    int triggerIndex; // The objIndex that corresponds to this task
};

class ChecklistSystem {
private:
    std::vector<ChecklistItem> items;

    void renderBitmapString(float x, float y, void *font, const char *string) {
        const char *c;
        glRasterPos2f(x, y);
        for (c = string; *c != '\0'; c++) {
            glutBitmapCharacter(font, *c);
        }
    }

public:
    ChecklistSystem() {
        items.push_back({"Remove Side Panel", REMOVE_SIDE_PANEL}); // 0
        items.push_back({"Disconnect RAM Sticks", REMOVE_RAM_STICK});     // 2
        items.push_back({"Unlock CPU Fan", REMOVE_FAN});           // 4
        items.push_back({"Remove Processor", REMOVE_PROCESSOR});   // 6
        items.push_back({"Unplug Power Supply", REMOVE_PSU});      // 8
        items.push_back({"Remove Hard Disk", REMOVE_HDD});         // 10
        items.push_back({"Detach GPU", REMOVE_GPU});               // 12
        items.push_back({"Remove Motherboard", REMOVE_MOTHERBOARD});// 14
    }

    void draw(int currentObjIndex, int windowWidth, int windowHeight) {
        // Find the index of the first incomplete task to highlight it
        int nextTaskIndex = -1;
        for (size_t i = 0; i < items.size(); ++i) {
             if (currentObjIndex < items[i].triggerIndex) {
                 nextTaskIndex = i;
                 break;
             }
        }
        // If everything is done (or logic fails), nothing is "next", but that's fine.

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST); // Ensure text draws on top

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, windowWidth, 0, windowHeight);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        float startX = 30.0f;
        float startY = windowHeight - 120.0f; // Moved down to clear FPS overlay
        float lineHeight = 30.0f;

        // Title Box Background
        glColor4f(0.1f, 0.1f, 0.1f, 0.8f);
        glRectf(startX - 10, startY + 25, startX + 300, startY - (items.size() * lineHeight) - 10);
        
        // Border
        glLineWidth(2.0f);
        glColor3f(0.0f, 0.8f, 1.0f); // Cyan
        glBegin(GL_LINE_LOOP);
            glVertex2f(startX - 10, startY + 25);
            glVertex2f(startX + 300, startY + 25);
            glVertex2f(startX + 300, startY - (items.size() * lineHeight) - 10);
            glVertex2f(startX - 10, startY - (items.size() * lineHeight) - 10);
        glEnd();

        // Title
        glColor3f(1.0f, 0.84f, 0.0f); // Gold
        renderBitmapString(startX, startY, GLUT_BITMAP_HELVETICA_18, "ASSEMBLY CHECKLIST");

        float currentY = startY - 35.0f;

        for (size_t i = 0; i < items.size(); ++i) {
            bool isCompleted = currentObjIndex >= items[i].triggerIndex;
            bool isNext = (i == nextTaskIndex);

            std::string prefix;
            if (isCompleted) {
                glColor3f(0.2f, 1.0f, 0.2f); // Bright Green
                prefix = "[x] ";
            } else if (isNext) {
                glColor3f(1.0f, 1.0f, 1.0f); // White/Highlight
                prefix = "[ ] ";
                // Draw a small arrow or indicator for the current task
                renderBitmapString(startX - 15, currentY, GLUT_BITMAP_HELVETICA_12, ">");
            } else {
                glColor3f(0.5f, 0.5f, 0.5f); // Grey
                prefix = "[ ] ";
            }

            renderBitmapString(startX, currentY, GLUT_BITMAP_HELVETICA_18, (prefix + items[i].text).c_str());
            currentY -= lineHeight;
        }

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING); // Restore lighting
    }
};

#endif
