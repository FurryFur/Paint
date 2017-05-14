#pragma once

class IShape;

class IUpdateListener
{
public:
	virtual ~IUpdateListener();

	virtual void NotifyUpdated(const IShape*) = 0;
protected:
	IUpdateListener();
};

