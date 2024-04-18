#pragma once
#include "CommonHeader.h"
#include <unicode/utf8.h>
#include <unicode/uchar.h>
#include <unicode/stringpiece.h>
#include <unicode/unistr.h>
#include <unicode/utypes.h>
#include <unicode/brkiter.h>


void ForEachGraphemeCluster(const char* src, std::function<void(std::string)> callback);

void ForEachGraphemeCluster(std::string src, std::function<void(std::string)> callback)
{
	ForEachGraphemeCluster(src.c_str(), callback);
}

void ForEachGraphemeCluster(const char* src, std::function<void(std::string)> callback)
{
	// Convert UTF-8 string to UnicodeString
	UErrorCode status = U_ZERO_ERROR;

	icu::UnicodeString unicodeString = icu::UnicodeString::fromUTF8(src);

	if (U_FAILURE(status))
	{
		std::cerr << "Error converting UTF-8 to UnicodeString" << u_errorName(status) << std::endl;
		return;
	}

	// Create a BreakIterator for grapheme clusters
	icu::BreakIterator* graphemeIterator = icu::BreakIterator::createCharacterInstance(icu::Locale::getDefault(), status);
	if (U_FAILURE(status))
	{
		std::cerr << "Error creating BreakIterator: " << u_errorName(status) << std::endl;
		return;
	}

	// Set the text for iteration
	graphemeIterator->setText(unicodeString);

	// Iterate over grapheme clusters
	for (int32_t start = graphemeIterator->first(), end = graphemeIterator->next(); end != icu::BreakIterator::DONE; start = end, end = graphemeIterator->next())
	{
		icu::UnicodeString grapheme = unicodeString.tempSubStringBetween(start, end);
		std::string utf8Grapheme{};
		grapheme.toUTF8String(utf8Grapheme);
		callback(utf8Grapheme);
	}
}
