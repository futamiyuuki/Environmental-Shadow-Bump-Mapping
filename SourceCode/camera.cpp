#include "camera.h"

const static float STEP_SCALE = 1.0f;
const static float EDGE_STEP = 0.5f;
const static int MARGIN = 10;

Camera::Camera(int WindowWidth, int WindowHeight, Skybox* sb)
{
    m_windowWidth  = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos          = Vector3f(0.0f, 0.0f, 0.0f);
    m_target       = Vector3f(0.0f, 0.0f, 1.0f);
    m_target.Normalize();
    m_up           = Vector3f(0.0f, 1.0f, 0.0f);

	skb = sb;

    Init();
}


Camera::Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up, Skybox* sb)
{
    m_windowWidth  = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Pos;

    m_target = Target;
    m_target.Normalize();

    m_up = Up;
    m_up.Normalize();

	skb = sb;

    Init();
}


void Camera::Init()
{
    Vector3f HTarget(m_target.x, 0.0, m_target.z);
    HTarget.Normalize();
    
    if (HTarget.z >= 0.0f)
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = 360.0f - ToDegree(asin(HTarget.z));
        }
        else
        {
            m_AngleH = 180.0f + ToDegree(asin(HTarget.z));
        }
    }
    else
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = ToDegree(asin(-HTarget.z));
        }
        else
        {
            m_AngleH = 90.0f + ToDegree(asin(-HTarget.z));
        }
    }
    
    m_AngleV = -ToDegree(asin(m_target.y));

    m_OnUpperEdge = false;
    m_OnLowerEdge = false;
    m_OnLeftEdge  = false;
    m_OnRightEdge = false;
	turnH = false;
	turnV = false;
    m_mousePos.x  = m_windowWidth / 2;
    m_mousePos.y  = m_windowHeight / 2;
	deltaX = 0;
	deltaY = 0;

   // glutWarpPointer(m_mousePos.x, m_mousePos.y);
}


bool Camera::OnKeyboard(OGLDEV_KEY Key)
{
    bool Ret = false;

    switch (Key) {

	case OGLDEV_KEY_W:
	case OGLDEV_KEY_w:
        {
            m_pos += (m_target * STEP_SCALE);
            Ret = true;
			Vector3f tpos = (m_target * STEP_SCALE);
			//skb->MoveSB(tpos.x, tpos.y, tpos.z);
        }
        break;
	case OGLDEV_KEY_S:
    case OGLDEV_KEY_s:
        {
            m_pos -= (m_target * STEP_SCALE);
            Ret = true;
			Vector3f tpos = (m_target * STEP_SCALE);
			//skb->MoveSB(-tpos.x, -tpos.y, -tpos.z);
        }
        break;

    case OGLDEV_KEY_A:
	case OGLDEV_KEY_a:
        {
            Vector3f Left = m_target.Cross(m_up);
            Left.Normalize();
            Left *= STEP_SCALE;
            m_pos += Left;
            Ret = true;
			Vector3f tpos = Left;
			//skb->MoveSB(tpos.x, tpos.y, tpos.z);
        }
        break;
    case OGLDEV_KEY_D:
	case OGLDEV_KEY_d:
        {
            Vector3f Right = m_up.Cross(m_target);
            Right.Normalize();
            Right *= STEP_SCALE;
            m_pos += Right;
            Ret = true;
			Vector3f tpos = Right;
			//skb->MoveSB(tpos.x, tpos.y, tpos.z);
        }
        break;

	case OGLDEV_KEY_I:
	case OGLDEV_KEY_i:
	{
						 Ret = true;
						 Vector3f tpos = (m_target * STEP_SCALE);
						 skb->MoveSB(tpos.x, tpos.y, tpos.z);
	}
		break;
	case OGLDEV_KEY_K:
	case OGLDEV_KEY_k:
	{
						 Ret = true;
						 Vector3f tpos = (m_target * STEP_SCALE);
						 skb->MoveSB(-tpos.x, -tpos.y, -tpos.z);
	}
		break;

	case OGLDEV_KEY_J:
	case OGLDEV_KEY_j:
	{
			Vector3f Left = m_target.Cross(m_up);
			Left.Normalize();
			Left *= STEP_SCALE;
			Ret = true;
			Vector3f tpos = Left;
			skb->MoveSB(tpos.x, tpos.y, tpos.z);
	}
		break;
	case OGLDEV_KEY_L:
	case OGLDEV_KEY_l:
	{
			Vector3f Right = m_up.Cross(m_target);
			Right.Normalize();
			Right *= STEP_SCALE;
			Ret = true;
			Vector3f tpos = Right;
			skb->MoveSB(tpos.x, tpos.y, tpos.z);
	}
		break;
        
	case OGLDEV_KEY_Q:
    case OGLDEV_KEY_q:
        m_pos.y += STEP_SCALE;
        break;
    
	case OGLDEV_KEY_Z:
    case OGLDEV_KEY_z:
        m_pos.y -= STEP_SCALE;
        break;

	//case OGLDEV_KEY_r:
		
    default:
        break;            
    }

    return Ret;
}


void Camera::OnMouse(int x, int y)
{
	deltaX = x - m_windowWidth / 2;
	deltaY = y - m_windowHeight / 2;

	if (x < m_windowWidth / 3 || (float)x > m_windowWidth * (2.2 / 3))
		turnH = true;
	else
		turnH = false;

	if (y < m_windowHeight / 3 || (float)y > m_windowHeight * (2.2 / 3))
		turnV = true;
	else
		turnV = false;

    Update();
}


void Camera::OnRender()
{
    bool ShouldUpdate = false;

    if (ShouldUpdate) {
        Update();
    }
}

void Camera::Update()
{
    const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

	if (turnH) {
		m_AngleH += deltaX / 400.0f;
	}

	if (turnV) {
		m_AngleV += deltaY / 400.0f;
	}

    Vector3f View(1.0f, 0.0f, 0.0f);
    View.Rotate(m_AngleH, Vaxis);
    View.Normalize();

    Vector3f Haxis = Vaxis.Cross(View);
    Haxis.Normalize();
    View.Rotate(m_AngleV, Haxis);
	
	if (turnH) {
		skb->RotSB((deltaX / 400.f), Vaxis.x, Vaxis.y, Vaxis.z);
	}

	if (turnV) {
		skb->RotSB((deltaY / 400.f), Haxis.x, Haxis.y, Haxis.z);
	}
       
    m_target = View;
    m_target.Normalize();

    m_up = m_target.Cross(Haxis);
    m_up.Normalize();
}
