#include <iostream>
#include "string"

using namespace std;


int main()
{
	string s, buf;
	const string pos_list = "([{)]}";
	int buf_len, pos, last_pos;
	
	cin >> s;
	buf = "";
	int l = s.length();

	for (int i = 0; i < l; i++)
	{
		pos = pos_list.find(s[i]);

		if ((pos > -1) and (pos < 3))
			buf += s[i];

		if (pos >= 3)
		{
			buf_len = buf.length();
			if (buf_len == 0)
			{
				buf = "error";
				break;
			}

			last_pos = pos_list.find(buf[buf_len-1]);

			if (pos - last_pos == 3)
				buf.erase(buf_len-1, 1);
			else
			{
				buf = "error";
				break;
			}
				
		}

	}

	if (buf == "")
		cout << "true" << endl;
	else
		cout << "false" << endl;

	return 0;
}