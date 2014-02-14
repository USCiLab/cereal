/* Copyright (c) 2014 Matthieu Tourne */

#ifndef CEREAL_TYPES_OPENCV_HPP_
#define CEREAL_TYPES_OPENCV_HPP_

#include <cereal/cereal.hpp>
#include <opencv2/opencv.hpp>

namespace cereal {

    // cv::Mat
    // this is largely inspired from opencv's persistence.cpp:icvWriteMat
    template<class Archive>
    void save(Archive &ar, const cv::Mat& m) {
        int elem_type = m.type();
        size_t step = m.step;
        size_t elem_size = m.elemSize();
        size_t elem_count = m.total();
        int y;

        ar( _CEREAL_NVP("rows", m.rows) );
        ar( _CEREAL_NVP("cols", m.cols) );

        ar( _CEREAL_NVP("elem_type", elem_type) );

        // type number and size of elements
        ar( _CEREAL_NVP("elem_size", elem_size) );
        ar( make_size_tag( elem_count) );

        cv::Size size = m.size();
        if(size.height > 0 && size.width > 0 && m.data) {
            if(CV_IS_MAT_CONT(elem_type)) {
                size.width *= size.height;
                size.height = 1;
            }

            for(y = 0; y < size.height; y++) {
                ar( binary_data(m.data + y * step, size.width * elem_size) );
            }
        }
    }

     template<class Archive>
     void load(Archive &ar, cv::Mat& m) {
         size_t elem_size, elem_count;
         int rows, cols, elem_type;

         ar( rows, cols, elem_type );

         m.create(rows, cols, elem_type);

         ar( elem_size );
         ar( make_size_tag(elem_count) );
         ar( binary_data(m.data, elem_count * elem_size));
     }

    // cv::Point
    template<class Archive, class T>
    void save(Archive& ar, const cv::Point_<T>& pt) {
        ar( _CEREAL_NVP("x", pt.x) );
        ar( _CEREAL_NVP("y", pt.y) );
    }

    template<class Archive, class T>
    void load(Archive& ar, cv::Point_<T>& pt) {
        T x, y;

        ar ( x, y );
        pt = cv::Point_<T>(x, y);
    }

    // cv::KeyPoint
    template<class Archive>
    void save(Archive& ar, const cv::KeyPoint& keypoint) {
        ar( _CEREAL_NVP("pt", keypoint.pt) );
        ar( _CEREAL_NVP("size", keypoint.size) );
        ar( _CEREAL_NVP("angle", keypoint.angle) );
        ar( _CEREAL_NVP("response", keypoint.response) );
        ar( _CEREAL_NVP("octave", keypoint.octave) );
        ar( _CEREAL_NVP("class_id", keypoint.class_id) );
    }

    template<class Archive>
    void load(Archive& ar, cv::KeyPoint& keypoint) {
        cv::Point2f pt;
        float size, angle, response;
        int octave, class_id;

        ar( pt, size, angle, response, octave, class_id );
        keypoint = cv::KeyPoint(pt, size, angle, response, class_id);
    }

    // cv::DMatch
    template<class Archive>
    void save(Archive& ar, const cv::DMatch& match) {
        ar( _CEREAL_NVP("query_idx", match.queryIdx) );
        ar( _CEREAL_NVP("train_idx", match.trainIdx) );
        ar( _CEREAL_NVP("train_idx", match.imgIdx) );

        ar( _CEREAL_NVP("distance", match.distance) );
    }

    template<class Archive>
    void load(Archive& ar, cv::DMatch& match) {
        int queryIdx, trainIdx, imgIdx;
        float distance;

        ar( queryIdx, trainIdx, imgIdx, distance);

        match = cv::DMatch(queryIdx, trainIdx, imgIdx, distance);
    }
}

#endif // !CEREAL_TYPOS_OPENCV_HPP_
