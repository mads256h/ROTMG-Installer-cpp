#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
struct Url {
public:
	Url(const std::string& url_s) {
		this->parse(url_s);
	}
	std::string protocol_, host_, path_, query_;
private:
	void parse(const std::string& url_s)
	{
		const std::string prot_end("://");
		std::string::const_iterator prot_i = search(url_s.begin(), url_s.end(),
			prot_end.begin(), prot_end.end());
		protocol_.reserve(distance(url_s.begin(), prot_i));
		transform(url_s.begin(), prot_i,
			back_inserter(protocol_),
			std::ptr_fun<int, int>(tolower)); // protocol is icase
		if (prot_i == url_s.end())
			return;
		advance(prot_i, prot_end.length());
		std::string::const_iterator path_i = find(prot_i, url_s.end(), '/');
		host_.reserve(distance(prot_i, path_i));
		transform(prot_i, path_i,
			back_inserter(host_),
			std::ptr_fun<int, int>(tolower)); // host is icase
		std::string::const_iterator query_i = find(path_i, url_s.end(), '?');
		path_.assign(path_i, query_i);
		if (query_i != url_s.end())
			++query_i;
		query_.assign(query_i, url_s.end());
	}
};