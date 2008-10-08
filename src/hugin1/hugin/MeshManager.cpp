// -*- c-basic-offset: 4 -*-
/** @file MeshManager.cpp
 *
 *  @author James Legg
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <wx/wx.h>
#include <wx/platform.h>

#ifdef __WXMAC__
#include <OpenGL/gl.h>
#else
#ifdef __WXMSW__
#include <vigra/windows.h>
#endif
#include <GL/gl.h>
#endif

#include "panoinc.h"
#include "ViewState.h"
#include "MeshManager.h"
#include "ChoosyRemapper.h"
#include <iostream>

// If we want to draw the outline of each face instead of shading it normally,
// uncomment this. Wireframe mode is for testing mesh quality.
// #define WIREFRAME

MeshManager::MeshManager(PT::Panorama *pano, ViewState *view_state_in)
{
    m_pano = pano;
    view_state = view_state_in;
}

MeshManager::~MeshManager()
{
    meshes.clear();
}

void MeshManager::CheckUpdate()
{
    // update what the view_state deems necessary.
    if (view_state->ImagesRemoved())
    {
        CleanMeshes();
        // we can't trust any meshes after images have been removed, they are
        // indexed by image number, which could have changed.
        unsigned int num_images = m_pano->getNrOfImages();
        for (unsigned int i = 0; i < num_images; i++)
        {
            UpdateMesh(i);
        }        
    } else {
        // check each image individualy.
        unsigned int num_images = m_pano->getNrOfImages();
        for (unsigned int i = 0; i < num_images; i++)
        {
            if (view_state->RequireRecalculateMesh(i))
            {
                UpdateMesh(i);
            }
        }
    }
}

void MeshManager::UpdateMesh(unsigned int image_number)
{
    // try to find if it is already here
    std::map<unsigned int, MeshInfo>::iterator mesh_info_itt = meshes.find(image_number);
    if (mesh_info_itt != meshes.end())
    {
        mesh_info_itt->second.Update();
    } else {
        // not found, make a new one
        DEBUG_INFO("Making new mesh remapper for image " << image_number << ".");
        meshes[image_number].SetSource(m_pano, image_number, view_state);
    }
}

void MeshManager::RenderMesh(unsigned int image_number)
{
    meshes[image_number].CallList();
}

unsigned int MeshManager::GetDisplayList(unsigned int image_number)
{
    return meshes[image_number].display_list_number;
}

void MeshManager::CleanMeshes()
{
    // remove meshes that we no longer need.
    // truncate the list to just the numbers we need.
    unsigned int num_images = m_pano->getNrOfImages();
    if (num_images > meshes.size())
    {
        meshes.erase(meshes.find(num_images), meshes.end());
    }
}

MeshManager::MeshInfo::MeshInfo()
{
    remap = 0;
    display_list_number = glGenLists(1);
}

void MeshManager::MeshInfo::SetSource(PT::Panorama *m_pano_in,
                                      unsigned int image_number_in,
                                      ViewState *view_state)
{
    image_number = image_number_in;
    m_pano = m_pano_in;
    DEBUG_ASSERT(m_pano);
    remap = new ChoosyRemapper(m_pano, image_number, view_state);
    DEBUG_ASSERT(remap);
    CompileList();
}

MeshManager::MeshInfo::~MeshInfo()
{
    glDeleteLists(display_list_number, 1);
    if (remap) delete remap;
}

void MeshManager::MeshInfo::Update()
{
    CompileList();
}

void MeshManager::MeshInfo::CallList()
{
    glCallList(display_list_number);
}


void MeshManager::MeshInfo::CompileList()
{
    // build the display list from the coordinates generated by the remapper
    DEBUG_INFO("Preparing to compile a display list for " << image_number
              << ".");
    DEBUG_ASSERT(remap);
    unsigned int number_of_faces = 0;
    glNewList(display_list_number, GL_COMPILE);
        remap->UpdateAndResetIndex();
        DEBUG_INFO("Specifying faces in display list.");
        #ifndef WIREFRAME
        glBegin(GL_QUADS);
        #endif
            // get each face's coordinates from the remapper
            MeshRemapper::Coords coords;
            while (remap->GetNextFaceCoordinates(&coords))
            {
                number_of_faces++;
                // go in an anticlockwise direction
                #ifdef WIREFRAME
                glBegin(GL_LINE_LOOP);
                #endif
                glTexCoord2dv(coords.tex_c[0][0]);
                glVertex2dv(coords.vertex_c[0][0]);
                glTexCoord2dv(coords.tex_c[0][1]);
                glVertex2dv(coords.vertex_c[0][1]);
                glTexCoord2dv(coords.tex_c[1][1]);
                glVertex2dv(coords.vertex_c[1][1]);
                glTexCoord2dv(coords.tex_c[1][0]);
                glVertex2dv(coords.vertex_c[1][0]);
                #ifdef WIREFRAME
                glEnd();
                #endif
            }
        #ifndef WIREFRAME
        glEnd();
        #endif
    glEndList();
    DEBUG_INFO("Prepared a display list for " << image_number << ", using "
              << number_of_faces << " face(s).");
}

