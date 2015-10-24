#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "engine_common.h"
#include "backend.h"
#include "pipeline.h"
#include "camera.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "mesh.h"
#include "shadow_map_technique.h"
#include "shadow_map_fbo.h"
#include "Skybox.h"


using namespace std;

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 1000
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

/*#define FRONT "Content/sffront.jpg"
#define BACK "Content/sfback.jpg"
#define TOP "Content/sfup.jpg"
#define BOTTOM "Content/sfbot.jpg"
#define LEFT "Content/sfleft.jpg"
#define RIGHT "Content/sfright.jpg"*/

#define FRONT "Content/negz.jpg"
#define BACK "Content/posz.jpg"
#define TOP "Content/posy.jpg"
#define BOTTOM "Content/negy.jpg"
#define LEFT "Content/negx.jpg"
#define RIGHT "Content/posx.jpg"

#define MESH_FILE "Content/suzanne.obj"



class Window : public ICallbacks
{
public:

    Window() 
    {
        m_pGameCamera = NULL;
        m_pLightingEffect = NULL;
        m_pShadowMapEffect = NULL;
		m_skb = NULL;
        m_scale = 0.0f;

		spotpos = -15;

        m_spotLight.AmbientIntensity = 0.9f;
        m_spotLight.DiffuseIntensity = 0.1f;
        m_spotLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_spotLight.Attenuation.Linear = 0.01f;
        m_spotLight.Position  = Vector3f(spotpos, 20.0, 1.0f);
        m_spotLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
        m_spotLight.Cutoff =  20.0f;

		m_pLight.AmbientIntensity = 0.1f;
		m_pLight.DiffuseIntensity = 0.9f;
		m_pLight.Color = Vector3f(0.4f, 1.0f, 1.0f);
		//m_pLight.Attenuation.Linear = 0.01f;
		m_pLight.Position = Vector3f(-1.0, 10.0, 1.0f);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;

		isBump = true;
		slight = true;
    }

    ~Window()
    {
        SAFE_DELETE(m_pLightingEffect);
        SAFE_DELETE(m_pShadowMapEffect);
        SAFE_DELETE(m_pGameCamera);
    }    

    bool Init()
    {
        Vector3f Pos(3.0f, 8.0f, -10.0f);
        Vector3f Target(0.0f, -0.5f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

		m_skb = new Skybox();

		m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up, m_skb);

        if (!m_shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up, m_skb);
      
        m_pLightingEffect = new LightingTechnique();

        if (!m_pLightingEffect->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pLightingEffect->Enable();
        m_pLightingEffect->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
        m_pLightingEffect->SetShadowMapTextureUnit(SHADOW_TEXTURE_UNIT_INDEX);
		m_pLightingEffect->SetNormalMapTextureUnit(NORMAL_TEXTURE_UNIT_INDEX);
        m_pLightingEffect->SetSpotLights(1, &m_spotLight);
        m_pLightingEffect->SetShadowMapSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);

		m_pLightingEffect->SetPointLights(1, &m_pLight);

        m_pShadowMapEffect = new ShadowMapTechnique();

        if (!m_pShadowMapEffect->Init()) {
            printf("Error initializing the shadow map technique\n");
            return false;
        }        

		if (!m_quad.LoadMesh("Content/quad.obj")) {
            return false;
        }

		m_pGroundTex = new Texture(GL_TEXTURE_2D, "Content/sandStone_C.png");

        if (!m_pGroundTex->Load()) {
            return false;
        }

		m_pGroundTexN = new Texture(GL_TEXTURE_2D, "Content/sandStone_N.png");

		if (!m_pGroundTexN->Load()) {
			return false;
		}

		m_pTrivialNormalMap = new Texture(GL_TEXTURE_2D, "Content/normal_up.jpg");

		if (!m_pTrivialNormalMap->Load()) {
			return false;
		}

		if (!m_mesh.LoadMesh("Content/pilaar.md2")) {
			return false;
		}
  
        return true;
    }

    void Run()
    {
        GLUTBackendRun(this);
    }
    

    virtual void RenderSceneCB()
    {   

        //m_scale += 0.05f;

        m_pGameCamera->OnRender();
;
        ShadowMapPass();
        RenderPass();
		m_skb->Render();

        glutSwapBuffers();
    }

    void ShadowMapPass()
    {
        glCullFace(GL_FRONT);
        
        m_shadowMapFBO.BindForWriting();

        glClear(GL_DEPTH_BUFFER_BIT);

        m_pShadowMapEffect->Enable();

        Pipeline p;
        p.Scale(0.1f, 0.1f, 0.1f);
		p.Rotate(0.0f, 90, 0.0f);
        p.WorldPos(0.0f, 0.0f, 3.0f);
		p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        p.SetPerspectiveProj(m_persProjInfo);
        m_pShadowMapEffect->SetWVP(p.GetWVPTrans());
        m_mesh.Render();
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
        
    void RenderPass()
    {
        glCullFace(GL_BACK);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_pGameCamera->Update();

		m_pLightingEffect->Enable();
             
        m_shadowMapFBO.BindForReading(GL_TEXTURE1);

        Pipeline p;
        p.SetPerspectiveProj(m_persProjInfo);           
        p.Scale(20.0f, 20.0f, 20.0f);
        p.WorldPos(0.0f, 0.0f, 1.0f);
        p.Rotate(90.0f, 0.0f, 0.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());

        m_pLightingEffect->SetWVP(p.GetWVPTrans());
        m_pLightingEffect->SetWorldMatrix(p.GetWorldTrans());
		p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        //p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        m_pLightingEffect->SetLightWVP(p.GetWVPTrans());
        m_pLightingEffect->SetEyeWorldPos(m_pGameCamera->GetPos());
		
		m_pGroundTex->Bind(GL_TEXTURE0);
		if (isBump)
		{
			m_pGroundTexN->Bind(GL_TEXTURE2);
		}
		else
		{
			m_pTrivialNormalMap->Bind(GL_TEXTURE2);
		}

		m_quad.Render();
 
        p.Scale(0.1f, 0.1f, 0.1f);
		p.Rotate(0.0f, 90.0f, 0.0f);
        p.WorldPos(0.0f, 0.0f, 3.0f);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        m_pLightingEffect->SetWVP(p.GetWVPTrans());
        m_pLightingEffect->SetWorldMatrix(p.GetWorldTrans());
		p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));

        //p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        m_pLightingEffect->SetLightWVP(p.GetWVPTrans());

		m_pTrivialNormalMap->Bind(GL_TEXTURE2);

        m_mesh.Render();
		m_pLightingEffect->Disable();
    }


	void KeyboardCB(OGLDEV_KEY OgldevKey)
	{
		switch (OgldevKey) {
		case OGLDEV_KEY_C:
		case OGLDEV_KEY_c:
			OgldevBackendLeaveMainLoop();
			break;
		case OGLDEV_KEY_B:
		case OGLDEV_KEY_b:
			isBump = !isBump;
			break;
		case OGLDEV_KEY_M:
		case OGLDEV_KEY_m:
		{
							 if (spotpos < -1) {
								 spotpos++;
							 }
							 else if (spotpos <= 1 && spotpos >= -1) {
								 spotpos++;
								 m_spotLight.Direction = Vector3f(0.0f, 0.0f, 0.0f);
							 }
							 else if (spotpos <= 20) {
								 spotpos++;
								 m_spotLight.Direction = Vector3f(-1.0f, -1.0f, 0.0f);
							 }
							 m_spotLight.Position = Vector3f(spotpos, 20.0, 1.0f);
							 break;
		}
		case OGLDEV_KEY_N:
		case OGLDEV_KEY_n:
		{
							 if (spotpos > 1) {
								 spotpos--;
							 }
							 else if (spotpos <= 1 && spotpos >= -1) {
								 spotpos--;
								 m_spotLight.Direction = Vector3f(0.0f, 0.0f, 0.0f);
							 }
							 else if (spotpos >= -20) {
								 spotpos--;
								 m_spotLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
							 }
							 m_spotLight.Position = Vector3f(spotpos, 20.0, 1.0f);
							 break;
		}
		default:
			m_pGameCamera->OnKeyboard(OgldevKey);
		}
	}


	virtual void PassiveMouseCB(int x, int y)
	{
		m_pGameCamera->OnMouse(x, y);
	}

    
private:
    
    LightingTechnique* m_pLightingEffect;
    ShadowMapTechnique* m_pShadowMapEffect;
	Skybox* m_skb;
    Camera* m_pGameCamera;
    float m_scale;
    SpotLight m_spotLight;
	PointLight m_pLight;
    Mesh m_mesh;
    Mesh m_quad;	
    PersProjInfo m_persProjInfo;
    Texture* m_pGroundTex;
	Texture* m_pGroundTexN;
	Texture* m_pTrivialNormalMap;
    ShadowMapFBO m_shadowMapFBO;
	bool isBump;
	bool slight;

	float spotpos;
};


int main(int argc, char** argv)
{
    Magick::InitializeMagick(*argv);
    GLUTBackendInit(argc, argv, true, false);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "Shadow&Bump")) {
        return 1;
    }
      
    Window window;

    if (!window.Init()) {
        return 1;
    }
        
    window.Run();

    return 0;
}