#pragma once

#include <queue>
#include <bitset>

class Keyboard
{
	friend class ShadowWindow;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event()
			:
			type(Type::Invalid),
			code(0u)
		{}
		Event(Type type, unsigned char code) noexcept
			:
			type (type),
			code(code)
		{}
		bool IsPressed() const noexcept
		{
			return type == Type::Press;
		}
		bool IsReleased()
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator = (const Keyboard&) = delete;
	// Key events
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event Readkey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void ClearKey() noexcept;
	// Char events
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	// Autorepeat control
	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool AutoRepeatIsEnabled() const noexcept;
private:
	void OnKeyPress(unsigned char keycode) noexcept;
	void OnKeyRelease(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autoRepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;

};