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

#include <algorithm>
#include <iostream>

#include <dds/sub/ddssub.hpp>
#include <dds/core/ddscore.hpp>
#include <rti/config/Logger.hpp>

#include "shapes.hpp"
#include "application.hpp"

void process_data(
        dds::sub::DataReader<::ShapeTypeExtended> reader,
        unsigned int& samples_read)
{
    // Take all samples
    dds::sub::LoanedSamples<::ShapeTypeExtended> samples = reader.take();
    for (auto sample : samples) {
        if (sample.info().valid()) {
            std::cout << "Read a effector control sample (\""
                      << sample.data().color()
                      << "\"), count: " << ++samples_read << std::endl;
        } else {
            std::cout << "Instance state changed to "
                      << sample.info().state().instance_state() << std::endl;
        }
    }
}  // The LoanedSamples destructor returns the loan

void run_subscriber_application(
        unsigned int domain_id,
        const std::string& qos_file,
        unsigned int sample_count)
{
    dds::core::QosProvider qos_provider(qos_file);

    dds::domain::DomainParticipant participant(
            domain_id,
            qos_provider.participant_qos());

    dds::topic::Topic<::ShapeTypeExtended> topic(
            participant,
            "effector_control");

    dds::sub::Subscriber subscriber(participant);
    dds::sub::DataReader<::ShapeTypeExtended> reader(
            subscriber,
            topic,
            qos_provider.datareader_qos("tsn_library::effector_profile"));

    // Create a ReadCondition for any data received on this reader and set a
    // handler to process the data
    unsigned int samples_read = 0;
    dds::sub::cond::ReadCondition read_condition(
            reader,
            dds::sub::status::DataState::any(),
            [reader, &samples_read]() { process_data(reader, samples_read); });

    // WaitSet will be woken when the attached condition is triggered
    dds::core::cond::WaitSet waitset;
    waitset += read_condition;

    std::cout << "::Effector control server waiting for connection"
              << std::endl;

    while (!application::shutdown_requested && samples_read < sample_count) {
        // Run the handlers of the active conditions. Wait for up to 1 second.
        waitset.dispatch(dds::core::Duration(1));
    }
}

int main(int argc, char *argv[])
{
    using namespace application;

    // Parse arguments and handle control-C
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
        run_subscriber_application(
                arguments.domain_id,
                arguments.qos_file,
                arguments.sample_count);
    } catch (const std::exception& ex) {
        std::cerr << "Exception in run_subscriber_application(): " << ex.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return EXIT_SUCCESS;
}
