#include "editorWindows/GameEditorViewport.h"
#include "nativeScripting/CppBuild.h"
#include "core/LevelEditorSystem.h"
#include "util/Settings.h"

#include "cocoa/util/Settings.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Application.h"
#include "cocoa/systems/RenderSystem.h"

#include <imgui.h>

namespace Cocoa
{
	namespace GameEditorViewport
	{
		// Internal variables
		static glm::vec2 m_GameviewPos = glm::vec2();
		static glm::vec2 m_GameviewSize = glm::vec2();
		static glm::vec2 m_GameviewMousePos = glm::vec2();
		static bool m_BlockEvents = false;

		void ImGui(SceneData& scene, bool* isWindowHovered)
		{
			static bool open = true;
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Game Editor Viewport", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
			*isWindowHovered = ImGui::IsWindowHovered();

			if (ImGui::BeginMenuBar())
			{
				static bool isPlaying = false;
				static bool compiling = false;
				static bool setCompilingFalse = false;

				if (ImGui::BeginMenu("Play", !isPlaying))
				{
					if (!isPlaying)
					{
						CPath tmpPath = Settings::General::engineAssetsPath;
						tmpPath.join(CPath::create("tmp.jade"));
						Scene::save(scene, tmpPath);
						Scene::play(scene);
						isPlaying = true;
					}
					ImGui::EndMenu();
				}
				else if (ImGui::BeginMenu("Stop", isPlaying))
				{
					if (isPlaying)
					{
						Scene::stop(scene);
						CPath tmpPath = Settings::General::engineAssetsPath;
						tmpPath.join(CPath::create("tmp.jade"));
						Scene::freeResources(scene);
						Scene::load(scene, tmpPath, false);
						isPlaying = false;
					}
					ImGui::EndMenu();
				}
				else if (ImGui::MenuItem("Compile"))
				{
					CPath scriptsPath = Settings::General::workingDirectory;
					scriptsPath.join(CPath::create("scripts"));
					CppBuild::Compile(scriptsPath);
				}
				ImGui::EndMenuBar();
			}

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 1);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1);

			ImVec2 windowSize = ImGui::GetContentRegionAvail() - ImVec2(1, 1);

			// Figure out the largest area that fits this target aspect ratio
			float aspectWidth = windowSize.x;
			float aspectHeight = (float)aspectWidth / Application::get()->getWindow()->getTargetAspectRatio();
			if (aspectHeight > windowSize.y)
			{
				// It doesn't fit our height, we must switch to pillarbox
				aspectHeight = windowSize.y;
				aspectWidth = (float)aspectHeight * Application::get()->getWindow()->getTargetAspectRatio();
			}

			// Center rectangle
			float vpX = (windowSize.x / 2.0f) - ((float)aspectWidth / 2.0f);
			float vpY = ((float)windowSize.y / 2.0f) - ((float)aspectHeight / 2.0f);

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(vpX, vpY));

			ImVec2 topLeft = ImGui::GetCursorScreenPos();
			m_GameviewPos.x = topLeft.x;
			m_GameviewPos.y = topLeft.y + aspectHeight;
			Input::setGameViewPos(m_GameviewPos);
			m_GameviewSize.x = aspectWidth;
			m_GameviewSize.y = aspectHeight;
			Input::setGameViewSize(m_GameviewSize);

			ImVec2 mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos() - ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
			m_GameviewMousePos.x = mousePos.x;
			m_GameviewMousePos.y = mousePos.y;
			Input::setGameViewMousePos(m_GameviewMousePos);

			Camera& camera = LevelEditorSystem::GetCamera();
			uint32 texId = NFramebuffer::getColorAttachment(camera.framebuffer, 0).graphicsId;
			ImGui::Image(reinterpret_cast<void*>(texId), ImVec2(aspectWidth, aspectHeight), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}
	}
}