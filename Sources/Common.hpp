
#ifndef COMMON_HPP
#define COMMON_HPP

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <glm.hpp>

extern GLFWwindow *GLWindow;
extern bool Dirty;
extern std::string RootDir;
extern std::string ResourceDir;
extern std::string InterfaceFile;

std::string GetInfo();
std::ifstream::pos_type GetFileSize(std::string Filename);
std::string FileOpen(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt);
std::string FileSave(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt);
std::vector<std::string> ListFiles(std::string Folder, std::string Extensions);
const char *UpperStr(std::string Text);
bool StrCompare(const std::string& Str1, const std::string& Str2);
void ParseVector(std::string Line, glm::vec3 &Vector);


#endif

