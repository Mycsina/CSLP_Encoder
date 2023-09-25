#pragma once
#include <opencv2/core/mat.hpp>

class Image
{
public:
	Image();
	~Image();
	bool load(const char* filename);
	void display_image();
	bool loaded() const { return m_loaded_; }
private:
	bool m_loaded_ = image_mat_.empty();
	cv::Mat image_mat_;	
};
