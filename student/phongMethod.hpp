/*!
 * @file
 * @brief This file contains phong rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *         Marek Filip, xfilip46@stud.fit.vutbr.cz
 * @date   2020-May-19
 */

#pragma once

#include <student/method.hpp>

/// \addtogroup cpu_side Úkoly v cpu části
/// @{

/**
 * @brief This class holds all variables of phong method.
 */
class PhongMethod: public Method{
  public:
    PhongMethod();
    virtual ~PhongMethod();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;


    /// \todo Zde si vytvořte proměnné, které budete potřebovat (id bufferů, programu, ...)
    VertexPullerID vao;
    ProgramID prg;
    BufferID vbo;  // vertex buffer for bunny
    BufferID ebo;  // index buffer for bunny
};

/// @}
