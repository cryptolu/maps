/******************************************************************************
 *
 * Progress bar (copied from by Boot 1.5)
 *
 ******************************************************************************/

#ifndef __PROGRESS_BAR_H__
#define __PROGRESS_BAR_H__

#include <string>
#include <fstream>
#include <iostream>

class Progress_bar
{
	private:
		std::ostream &m_os;
		const std::string m_s1;
		const std::string m_s2;
		const std::string m_s3;
		unsigned long int _count;
		unsigned long int _expected_count;
		unsigned long int _next_tic_count;
		unsigned int _tic;

		void display_tic(void);

	public:
		explicit Progress_bar(
			unsigned long expected_count,
			std::ostream & os = std::cout,
			const std::string &s1 = "\n", //leading strings
			const std::string &s2 = "",
			const std::string &s3 = ""
		);

	void restart(unsigned long expected_count);
	unsigned long operator+=(unsigned long increment);
	unsigned long operator++();
	unsigned long count() const;
	unsigned long expected_count() const;


};

#endif
