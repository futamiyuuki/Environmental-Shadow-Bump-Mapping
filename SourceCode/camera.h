#ifndef CAMERA_H
#define	CAMERA_H

#include "math_3d.h"
#include "oglkey.h"
#include "Skybox.h"

class Camera
{
public:

    Camera(int WindowWidth, int WindowHeight, Skybox* sb);

    Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up, Skybox* sb);

    bool OnKeyboard(OGLDEV_KEY Key);

    void OnMouse(int x, int y);

    void OnRender();

	void Update();

    const Vector3f& GetPos() const
    {
        return m_pos;
    }

    const Vector3f& GetTarget() const
    {
        return m_target;
    }

    const Vector3f& GetUp() const
    {
        return m_up;
    }

private:

    void Init();
	//void Update();

    Vector3f m_pos;
    Vector3f m_target;
    Vector3f m_up;

    float m_windowWidth;
    float m_windowHeight;
	float deltaX;
	float deltaY;

    float m_AngleH;
    float m_AngleV;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

	bool turnH;
	bool turnV;

    Vector2i m_mousePos;

	Skybox* skb;
};

#endif	/* CAMERA_H */

