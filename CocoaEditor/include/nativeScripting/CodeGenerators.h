#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "nativeScripting/ScriptParser.h"

#include "cocoa/file/File.h"
#include "cocoa/util/Log.h"
#include "cocoa/util/Settings.h"

#include <filesystem>

namespace Cocoa
{
	namespace CodeGenerators
	{
		static CPath visitedClassBuffer[100];
		static int numVisited = 0;

		static bool visitedSourceFile(UClass clazz)
		{
			for (int i = 0; i < numVisited; i++)
			{
				if (clazz.m_FullFilepath == visitedClassBuffer[i])
				{
					return true;
				}
			}

			return false;
		}

		void GenerateInitFile(const std::vector<UClass>& classes, const CPath& filepath)
		{
			std::ostringstream source;

			source << "#pragma once\n";
			source << "#include \"cocoa/core/Core.h\"\n";
			source << "#include \"cocoa/util/Log.h\"\n";
			source << "#include \"cocoa/core/Entity.h\"\n";
			source << "#include \"cocoa/core/EntityStruct.h\"\n";

			const std::filesystem::path base = NCPath::GetDirectory(filepath, -1);
			for (auto clazz : classes)
			{
				const std::filesystem::path otherPath = clazz.m_FullFilepath.Path.c_str();
				source << "#include \"" << std::filesystem::relative(otherPath, base).generic_string().c_str() << "\"\n";

				std::string genFilename = NCPath::GetFilenameWithoutExt(clazz.m_FullFilepath) + "-generated" + NCPath::FileExt(clazz.m_FullFilepath);
				CPath otherGenCPath = NCPath::CreatePath(NCPath::GetDirectory(clazz.m_FullFilepath, -1));
				NCPath::Join(otherGenCPath, NCPath::CreatePath("generated"));
				NCPath::Join(otherGenCPath, NCPath::CreatePath(genFilename));
				const std::filesystem::path otherGenPath = otherGenCPath.Path.c_str();
				source << "#include \"" << std::filesystem::relative(otherGenPath, base).generic_string().c_str() << "\"\n\n";
			}

			source << "#include \"cocoa/components/TransformStruct.h\"\n";
			source << "#include \"cocoa/components/Tag.h\"\n";
			source << "#include \"cocoa/components/SpriteRenderer.h\"\n";
			source << "#include \"cocoa/components/FontRenderer.h\"\n";
			source << "#include \"cocoa/physics2d/PhysicsComponents.h\"\n";

			source << "\n";
			source << "#define ENTT_STANDARD_CPP\n";
			source << "#include <entt/entt.hpp>\n";
			source << "#include <nlohmann/json.hpp>\n\n";

			source << "extern \"C\" namespace Cocoa\n";
			source << "{\n";
			source << "\textern \"C\" namespace Init\n";
			source << "\t{\n";

			// Init Component Id's -------------------------------------------------------------------------------------------
			source << "\t\tstatic void InitComponentIds(SceneData & scene)\n";
			source << "\t\t{\n";
			source << "\t\t\tNEntity::RegisterComponentType<TransformData>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Tag>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<SpriteRenderer>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<FontRenderer>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Rigidbody2D>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Box2D>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<Circle>();\n";
			source << "\t\t\tNEntity::RegisterComponentType<AABB>();\n";

			for (auto clazz : classes)
			{
				source << "\t\t\tNEntity::RegisterComponentType<" << clazz.m_ClassName.c_str() << ">();\n";
			}

			source << "\t\t}\n";

			// AddComponent function
			source << "\t\textern \"C\" COCOA_SCRIPT void AddComponent(entt::registry& registryRef, std::string className, entt::entity entity)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Assert(registryRef.valid(entity), \"Invalid entity in script\");\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz)) 
				{
					std::string namespaceName = "Reflect" + ScriptParser::GetFilenameAsClassName(NCPath::GetFilenameWithoutExt(clazz.m_FullFilepath));
					source << "\t\t\tfor (auto strClass : " << namespaceName.c_str() << "::stringToMap)\n";
					source << "\t\t\t{\n";
					source << "\t\t\t\tif (strClass.first == className)\n";
					source << "\t\t\t\t{\n";
					source << "\t\t\t\t\t" << namespaceName << "::AddComponent(className, entity, registryRef);\n";
					source << "\t\t\t\t\treturn;\n";
					source << "\t\t\t\t}\n";
					source << "\t\t\t}\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}

			source << "\t\t}\n\n";

			// Generate UpdateScripts function
			source << "\t\textern \"C\" COCOA_SCRIPT void UpdateScripts(entt::registry& registryRef, float dt)\n";
			source << "\t\t{\n";
			for (auto clazz : classes)
			{
				source << "\t\t\t{\n";
				source << "\t\t\t\tauto view = registryRef.view<" << clazz.m_ClassName.c_str() << ">();\n";
				source << "\t\t\t\tfor (auto entity : view)\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tauto comp = registryRef.get<" << clazz.m_ClassName.c_str() << ">(entity);\n";
				source << "\t\t\t\t\tcomp.Update(NEntity::CreateEntity(entity), dt);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";
			}
			source << "\t\t}\n";

			// Generate EditorUpdateScripts function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void EditorUpdateScripts(entt::registry& registryRef, float dt)\n";
			source << "\t\t{\n";
			for (auto clazz : classes)
			{
				source << "\t\t\t{\n";
				source << "\t\t\t\tauto view = registryRef.view<" << clazz.m_ClassName.c_str() << ">();\n";
				source << "\t\t\t\tfor (auto entity : view)\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\tauto comp = registryRef.get<" << clazz.m_ClassName.c_str() << ">(entity);\n";
				source << "\t\t\t\t\tcomp.EditorUpdate(NEntity::CreateEntity(entity), dt);\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";
			}
			source << "\t\t}\n";

			// Generate SaveScript function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void SaveScripts(entt::registry& registryRef, json& j, SceneData* sceneData)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Saving scripts\");\n";
			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::GetFilenameAsClassName(NCPath::GetFilenameWithoutExt(clazz.m_FullFilepath));
					source << "\t\t\t" << namespaceName.c_str() << "::SaveScripts(j, registryRef, sceneData);\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Load Scripts function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void LoadScript(entt::registry& registryRef, json& j, Entity entity)\n";
			source << "\t\t{\n";
			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::GetFilenameAsClassName(NCPath::GetFilenameWithoutExt(clazz.m_FullFilepath));
					source << "\t\t\t" << namespaceName.c_str() << "::TryLoad(j, entity, registryRef);\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Init Scripts function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void InitScripts(SceneData* sceneData)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Initializing scripts\");\n";
			source << "\t\t\tInitComponentIds(*sceneData);\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::GetFilenameAsClassName(NCPath::GetFilenameWithoutExt(clazz.m_FullFilepath));
					source << "\t\t\t" << namespaceName.c_str() << "::Init();\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Init ImGui function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void InitImGui(void* ctx)\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Initializing ImGui\");\n";
			source << "\t\t\tImGui::SetCurrentContext((ImGuiContext*)ctx);\n";
			source << "\t\t}\n";

			// Generate ImGui function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void ImGui(entt::registry& registryRef, Entity entity)\n";
			source << "\t\t{\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::GetFilenameAsClassName(NCPath::GetFilenameWithoutExt(clazz.m_FullFilepath));
					source << "\t\t\t" << namespaceName.c_str() << "::ImGui(entity, registryRef);\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			// Generate Delete Scripts function
			source << "\n";
			source << "\t\textern \"C\" COCOA_SCRIPT void DeleteScripts()\n";
			source << "\t\t{\n";
			source << "\t\t\tLog::Info(\"Deleting Scripts\");\n";

			numVisited = 0;
			for (auto clazz : classes)
			{
				if (!visitedSourceFile(clazz))
				{
					std::string namespaceName = "Reflect" + ScriptParser::GetFilenameAsClassName(NCPath::GetFilenameWithoutExt(clazz.m_FullFilepath));
					source << "\t\t\t" << namespaceName.c_str() << "::DeleteScripts();\n";

					visitedClassBuffer[numVisited] = clazz.m_FullFilepath;
					numVisited++;
				}
			}
			source << "\t\t}\n";

			source << "\t}\n";
			source << "}\n";

			File::WriteFile(source.str().c_str(), filepath);
		}

		void GeneratePremakeFile(const CPath& filepath)
		{
			std::ostringstream stream;
			stream << ""
		   "workspace \"ScriptingWorkspace\"\n"
		   "	architecture \"x64\"\n"
		   "\n"
		   "	configurations {\n"
		   "		\"Debug\",\n"
		   "		\"Release\",\n"
		   "		\"Dist\"\n"
		   "	}\n"
		   "\n"
		   "	startproject \"ScriptModule\"\n"
		   "\n"
		   "	project \"ScriptModule\"\n"
		   "	kind \"SharedLib\"\n"
		   "	language \"C++\"\n"
		   "	cppdialect \"C++17\"\n"
		   "	staticruntime \"off\"\n"
		   "\n"
		   "	targetdir(\"bin/%{prj.name}\")\n"
		   "	objdir(\"bin-int/%{prj.name}\")\n"
		   "\n"
		   "	files {\n"
		   "		\"**.h\",\n"
		   "		\"**.cpp\",\n"
		   "		\"**.hpp\",\n";
			
			CPath engineSource = NCPath::CreatePath(Settings::General::s_EngineSourceDirectory.Path.c_str(), true);
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEditor/cpp/gui/**.cpp\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEditor/include/gui/**.h\"\n";
			
			stream << ""
		   "	}\n"
		   "\n"
		   "	disablewarnings {\n"
		   "		\"4251\"\n"
		   "	}\n"
		   "\n"
		   "	includedirs {\n"
		   "		\"%{prj.name}/scripts\",\n";

			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/include\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/vendor\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/vendor/glmVendor\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/vendor/enttVendor/src\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/vendor/glad/include\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/vendor/imguiVendor\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/vendor/box2DVendor/include\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/vendor/nlohmann-json/single_include\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEngine/vendor/GLFW/include\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineSource).c_str() << "/CocoaEditor/include/gui\"\n";
			
			stream << ""
		   "	}\n"
		   "\n"
		   "	symbolspath '$(OutDir)$(TargetName)-$([System.DateTime]::Now.ToString(\"HH_mm_ss_fff\")).pdb'\n"
		   "\n"
		   "	links {\n";

			CPath engineExeDir = NCPath::CreatePath(Settings::General::s_EngineExeDirectory.Path.c_str(), true);
			CPath engineDllDir = NCPath::CreatePath(NCPath::GetDirectory(Settings::General::s_EngineExeDirectory, -1), true);
			NCPath::Join(engineDllDir, NCPath::CreatePath("CocoaEngine"));
			stream << "\t\t\"" << NCPath::LinuxStyle(engineDllDir).c_str() << "/CocoaEngine.lib\",\n";
			stream << "\t\t\"" << NCPath::LinuxStyle(engineExeDir).c_str() << "/ImGui.lib\"\n";
			
			stream << ""
		   "	}\n"
		   "\n"
		   "	filter \"system:windows\"\n"
		   "\n"
		   "	defines {\n"
		   "		\"_COCOA_SCRIPT_DLL\"\n"
		   "	}\n"
		   "\n"
		   "	buildcommands {\n"
		   "		\"del /S *.pdb\"\n"
		   "	}\n"
		   "\n"
		   "	postbuildcommands {\n";

			stream << "\t\t\"copy /y \\\"$(SolutionDir)bin\\\\ScriptModule\\\\ScriptModule.dll\\\" \\\"" << NCPath::LinuxStyle(engineExeDir).c_str() << "/ScriptModuleTmp.dll\\\"\"\n";
				
			stream << ""
		   "	}\n"
		   "\n"
		   "	filter \"configurations:Debug\"\n"
		   "		runtime \"Debug\"\n"
		   "		symbols \"on\"\n"
		   "\n"
		   "\n"
		   "	filter \"configurations:Release\"\n"
		   "		runtime \"Release\"\n"
		   "		optimize \"on\"\n"
		   "\n"
		   "\n"
		   "	filter \"configurations:Dist\"\n"
		   "		runtime \"Release\"\n"
		   "		optimize \"on\"\n";

			File::WriteFile(stream.str().c_str(), filepath);
		}
	};
}