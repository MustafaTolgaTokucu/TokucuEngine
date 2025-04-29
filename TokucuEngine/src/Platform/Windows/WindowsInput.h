#pragma once

#include "Tokucu/Input.h"


namespace Tokucu {

	class WindowsInput : public Input
	{
	public:

	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;

		virtual float GetMOuseXImpl() override;
		virtual std::pair<float, float> GetMousePositionImpl();
		virtual float GetMouseYImpl() override;

	};


}
