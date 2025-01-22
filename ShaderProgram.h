class ShaderProgram {
public:
    unsigned int ID;

    ShaderProgram() {
        ID = glCreateProgram();
    }

    void attachShader(const Shader& shader) {
        glAttachShader(ID, shader.ID);
    }

    void link() {
        glLinkProgram(ID);

        int success;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            std::cerr << "Error: Program linking failed\n" << infoLog << std::endl;
        }
    }

    void use() const {
        glUseProgram(ID);
    }
    
    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    ~ShaderProgram() {
        glDeleteProgram(ID);
    }
};