/*
 * glsl_shader.h
 *
 *  Created on: Nov 27, 2011
 *      Author: Hordur Johannsson
 */

#ifndef PCL_SIMULATION_GLSL_SHADER
#define PCL_SIMULATION_GLSL_SHADER

#include <pcl/pcl_exports.h>

#include <GL/glew.h>
#include <Eigen/Core>
#include <boost/shared_ptr.hpp>

namespace pcl {
namespace simulation {
namespace gllib {
enum ShaderType {
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    TESS_CONTROL = GL_TESS_CONTROL_SHADER,
    TESS_EVALUATION = GL_TESS_EVALUATION_SHADER
};

/**
 * A GLSL shader program.
 */
class PCL_EXPORTS Program {
  public:
    typedef boost::shared_ptr<Program> Ptr;
    typedef boost::shared_ptr<const Program> ConstPtr;

    /**
     * Construct an empty shader program.
     */
    Program();

    /**
     * Destruct the shader program.
     */
    ~Program();

    /**
     * Add a new shader object to the program.
     */
    bool addShaderText(const std::string &text, ShaderType shader_type);

    /**
     * Add a new shader object to the program.
     */
    bool addShaderFile(const std::string &text, ShaderType shader_type);

    /**
     * Link the program.
     */
    bool link();

    /**
     * Return true if the program is linked.
     */
    bool isLinked();

    /**
     * Use the program.
     */
    void use();

    // Set uniforms
    void setUniform(const std::string &name, const Eigen::Vector2f &v);
    void setUniform(const std::string &name, const Eigen::Vector3f &v);
    void setUniform(const std::string &name, const Eigen::Vector4f &v);
    void setUniform(const std::string &name, const Eigen::Matrix2f &v);
    void setUniform(const std::string &name, const Eigen::Matrix3f &v);
    void setUniform(const std::string &name, const Eigen::Matrix4f &v);
    void setUniform(const std::string &name, float v);
    void setUniform(const std::string &name, int v);
    void setUniform(const std::string &name, bool v);

    int getUniformLocation(const std::string &name);

    void printActiveUniforms();
    void printActiveAttribs();

    GLuint programId() { return program_id_; }

    static Ptr loadProgramFromFile(const std::string &vertex_shader_file,
                                   const std::string &fragment_shader_file);
    static Ptr loadProgramFromText(const std::string &vertex_shader_text,
                                   const std::string &fragment_shader_text);

  private:
    GLuint program_id_;
};

GLenum PCL_EXPORTS getGLError();
void PCL_EXPORTS printShaderInfoLog(GLuint shader);
void PCL_EXPORTS printProgramInfoLog(GLuint program);

} // namespace gllib
} // namespace simulation
} // namespace pcl

#endif /* PCL_SIMULATION_GLSL_SHADER_HPP_ */
