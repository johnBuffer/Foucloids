#pragma once

template<typename T>
class Transition
{
public:
	Transition() :
		m_current_value(),
		m_target_value(),
		m_speed(0.0f)
	{}

	Transition(const T& value, float speed=1.0f) :
		m_current_value(value),
		m_target_value(value),
		m_speed(speed)
	{}

	template<typename... Args>
	explicit Transition(Args&&... args) :
		m_current_value(std::forward<Args>(args)...),
		m_target_value(m_current_value),
		m_speed(1.0f)
	{}

	operator const T&() const
	{
		return m_current_value;
	}

	void update(float dt = 0.016f)
	{
		m_current_value += (m_target_value - m_current_value) * (m_speed * dt);
	}

	void operator=(const T& value)
	{
		m_target_value = value;
	}

	void speed(float s)
	{
		m_speed = s;
	}

private:
	T m_current_value;
	T m_target_value;

	float m_speed;
};
