#pragma once

class Triangle;

class EditorPanel
{
public:
	void SetTriangle(Triangle* editor)
	{
		m_Editor = editor;
	}
protected:
	Triangle* m_Editor = nullptr;

};