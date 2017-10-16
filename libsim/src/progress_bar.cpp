/******************************************************************************
 *
 * Progress bar (heavily inpsired by Boot 1.5)
 *
 ******************************************************************************/

#include <fstream>
#include <iostream>
#include <string>

#include "progress_bar.h"


Progress_bar::Progress_bar(
	unsigned long expected_count,
	std::ostream &os,
	const std::string &s1,
	const std::string &s2,
	const std::string &s3
	) :  m_os(os), m_s1(s1), m_s2(s2), m_s3(s3)
{
	restart(expected_count);
}

void Progress_bar::restart(unsigned long expected_count)
{
	this->_count = 0;
	this->_next_tic_count =0;
	this->_tic = 0;
	this->_expected_count = expected_count;

    m_os << m_s1 << "0%   10   20   30   40   50   60   70   80   90   100%\n"
         << m_s2 << "|----|----|----|----|----|----|----|----|----|----|"
         << std::endl  // endl implies flush, which ensures display
         << m_s3;
    if (!_expected_count)
	{
		this->_expected_count = 1;  // prevent divide by zero
	}
}

unsigned long Progress_bar::operator+=(unsigned long increment)
{
    if ((this->_count += increment) >= this->_next_tic_count)
	{
		this->display_tic();
	}
    return this->_count;
}

unsigned long Progress_bar::operator++()
{
	return this->operator+=(1);
}

unsigned long Progress_bar::count() const
{
	return this->_count;
}
unsigned long Progress_bar::expected_count() const
{
	return this->_expected_count;
}

void Progress_bar::display_tic()
{
	unsigned int tics_needed = static_cast<unsigned int>((static_cast<double>(_count)/_expected_count)*50.0);
	do
	{
		m_os << '*' << std::flush;
	} while (++_tic < tics_needed);
	_next_tic_count = static_cast<unsigned long>((_tic/50.0)*_expected_count);
	if (_count == _expected_count)
	{
		if (_tic < 51)
		{
			m_os << '*';
		}
		m_os << std::endl;
	}
}
