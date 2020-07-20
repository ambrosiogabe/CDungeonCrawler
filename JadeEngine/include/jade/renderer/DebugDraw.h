#pragma once
#include "externalLibs.h"

#include "jade/renderer/Line2D.h"
#include "jade/renderer/Shader.h"
#include "jade/renderer/RenderBatch.h"

namespace Jade {
    class DebugDraw {
    public:
        static void BeginFrame();
        static void EndFrame();

        static void AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth=1.0f, glm::vec3 color={0.0f, 1.0f, 0.0f}, int lifetime=1);
        static void AddBox2D(glm::vec2& center, glm::vec2& dimensions, float rotation=0.0f, float strokeWidth=1.0f, glm::vec3 color={0.0f, 1.0f, 0.0f}, int lifetime=1);
        static void AddSprite(uint32 texId, glm::vec2 size, 
            glm::vec3 tint={1.0f, 1.0f, 1.0f}, glm::vec2 texCoordMin={0.0f, 1.0f}, glm::vec2 texCoordMax={1.0f, 0.0f}, float rotation=0.0f, int lifetime=1);

    private:        
        static std::vector<RenderBatch*> m_Batches;
        static std::vector<Line2D> m_Lines;
        static Shader* m_Shader;
        static int m_MaxBatchSize;
    };
}