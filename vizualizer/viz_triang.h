#ifndef _VIZ_TRINAG_H
#define _VIZ_TRINAG_H
/*$Id: viz_triang.h 1620 2007-02-18 15:24:21Z manwe $*/

/* Copyright (c) 2005, 2008, 2009 Alexey Ozeritsky (������� ���������)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Alexey Ozeritsky.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <vector>
#include <string>

#include "viz_obj.h"

/* 
   ����������� �� �������������
   �������� �� ����� � �������
      x y z
	  x y z
	  #
	  n1 n2 n3
*/
class Viz_Triang: public Viz_Obj {
public:
	Viz_Triang(const char * file);

	virtual ~Viz_Triang();
	void draw();
	void keyPressEvent1 ( unsigned char key, int x, int y );

private:
	std::string fname_;
	std::vector < Viz_Point > points_;     //!<���������� �����
	std::vector < std::vector < int > > nodes_; //!<������ �����

	void load_file();
	void normalize();
	void gen_lists();

	unsigned int fill_; // filled
	unsigned int wire_; // wireframe
};

#endif //_VIZ_TRINAG_H
