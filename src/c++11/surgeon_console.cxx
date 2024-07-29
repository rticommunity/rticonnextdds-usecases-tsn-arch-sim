/*
 * (c) Copyright, Real-Time Innovations, 2024.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

#include <iostream>
#include <cmath>

#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp>
#include <rti/config/Logger.hpp>

#include "application.hpp"
#include "shapes.hpp"

void run_publisher_application(
        unsigned int domain_id,
        const std::string& qos_file,
        unsigned int sample_count)
{
    dds::core::QosProvider qos_provider(qos_file);

    dds::domain::DomainParticipant participant(
            domain_id,
            qos_provider.participant_qos());

    dds::topic::Topic<::ShapeTypeExtended> video_topic(
            participant,
            "video_control");
    dds::topic::Topic<::ShapeTypeExtended> effector_topic(
            participant,
            "effector_control");

    dds::pub::Publisher publisher(participant);

    dds::pub::DataWriter<::ShapeTypeExtended> video_writer(
            publisher,
            video_topic,
            qos_provider.datawriter_qos("tsn_library::video_profile"));
    dds::pub::DataWriter<::ShapeTypeExtended> effector_writer(
            publisher,
            effector_topic,
            qos_provider.datawriter_qos("tsn_library::effector_profile"));

    ::ShapeTypeExtended data;

    const int left = 15, top = 15, right = 248, bottom = 278;  // limits
    const int shape_size = 30;
    int x = left - shape_size, y = bottom - top / 2;
    const float AMPLITUDE = 100.0f;
    const float FREQUENCY = 0.0475f;

    data.shapesize(shape_size);
    data.fillKind(ShapeFillKind::SOLID_FILL);

    // Main loop, write data
    unsigned int samples_written = 0;
    for (; !application::shutdown_requested && samples_written < sample_count;
         ++samples_written) {
        if (++x > right)
            x = left - shape_size;

        y = (int) (bottom - top) / 2 + AMPLITUDE * std::sin(FREQUENCY * x);

        data.x(x);
        data.y(y);

        std::cout << "Writing a video (ORANGE) and effector (CYAN) control "
                     "sample, count: "
                  << samples_written << std::endl;

        data.color("ORANGE");
        video_writer.write(data);
        data.color("CYAN");
        effector_writer.write(data);

        rti::util::sleep(dds::core::Duration(1));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    auto arguments = parse_arguments(argc, argv);
    if (arguments.parse_result == ParseReturn::exit) {
        return EXIT_SUCCESS;
    } else if (arguments.parse_result == ParseReturn::failure) {
        return EXIT_FAILURE;
    }
    setup_signal_handlers();

    // Sets Connext verbosity to help debugging
    rti::config::Logger::instance().verbosity(arguments.verbosity);

    try {
        run_publisher_application(
                arguments.domain_id,
                arguments.qos_file,
                arguments.sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in run_publisher_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
