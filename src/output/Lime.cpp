/*
   Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Her Majesty the
   Queen in Right of Canada (Communications Research Center Canada)

   Copyright (C) 2018
   Evariste F5OEO, evaristec@gmail.com

    
DESCRIPTION:
   It is an output driver using the LimeSDR library.
*/

/*
   This file is part of ODR-DabMod.

   ODR-DabMod is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   ODR-DabMod is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ODR-DabMod.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "output/Lime.h"

#ifdef HAVE_LIMESDR

//#include <SoapySDR/Errors.hpp>
#include <chrono>
#include <limits>
#include <cstdio>
#include <iomanip>

#include "Log.h"
#include "Utils.h"

using namespace std;

namespace Output {

static constexpr size_t FRAMES_MAX_SIZE = 2;

Lime::Lime(SDRDeviceConfig& config) :
    SDRDevice(),
    m_conf(config)
{
    etiLog.level(info) <<
        "Lime:Creating the device with: " <<
        m_conf.device;
    int device_count = LMS_GetDeviceList(NULL);
	if (device_count < 0)
	{
		etiLog.level(error) << "Error making LimeSDR device: " << LMS_GetLastErrorMessage();
        throw std::runtime_error("Cannot find LimeSDR output device");
		
	}
    lms_info_str_t device_list[device_count];
	if (LMS_GetDeviceList(device_list) < 0)
	{
		etiLog.level(error) << "Error making LimeSDR device: %s " << LMS_GetLastErrorMessage();
        throw std::runtime_error("Cannot find LimeSDR output device");
		
	}
    unsigned int device_i=0; // If several cards, need to get device by configuration
    if (LMS_Open(&m_device, device_list[device_i], NULL) < 0)
	{
		etiLog.level(error) << "Error making LimeSDR device: %s " << LMS_GetLastErrorMessage();
        throw std::runtime_error("Cannot open LimeSDR output device");
	}
    if (LMS_Reset(m_device) < 0)
	{
		etiLog.level(error) << "Error making LimeSDR device: %s " << LMS_GetLastErrorMessage();
        throw std::runtime_error("Cannot reset LimeSDR output device");
	}

	if (LMS_Init(m_device) < 0)
	{
		etiLog.level(error) << "Error making LimeSDR device: %s " << LMS_GetLastErrorMessage();
        throw std::runtime_error("Cannot init LimeSDR output device");
	}

    
    /*
    m_device->setMasterClockRate(m_conf.masterClockRate);
    etiLog.level(info) << "SoapySDR master clock rate set to " <<
        std::fixed << std::setprecision(4) <<
        m_device->getMasterClockRate()/1000.0 << " kHz";

    m_device->setSampleRate(SOAPY_SDR_TX, 0, m_conf.sampleRate);
    etiLog.level(info) << "SoapySDR:Actual TX rate: " <<
        std::fixed << std::setprecision(4) <<
        m_device->getSampleRate(SOAPY_SDR_TX, 0) / 1000.0 <<
        " ksps.";

    tune(m_conf.lo_offset, m_conf.frequency);
    m_conf.frequency = m_device->getFrequency(SOAPY_SDR_TX, 0);
    etiLog.level(info) << "SoapySDR:Actual frequency: " <<
        std::fixed << std::setprecision(3) <<
        m_conf.frequency / 1000.0 << " kHz.";

    m_device->setGain(SOAPY_SDR_TX, 0, m_conf.txgain);
    etiLog.level(info) << "SoapySDR:Actual tx gain: " <<
        std::fixed << std::setprecision(2) <<
        m_device->getGain(SOAPY_SDR_TX, 0);

    if (not m_conf.tx_antenna.empty()) {
        m_device->setAntenna(SOAPY_SDR_TX, 0, m_conf.tx_antenna);
    }
    etiLog.level(info) << "SoapySDR:Actual tx antenna: " <<
        m_device->getAntenna(SOAPY_SDR_TX, 0);

    if (m_device->hasHardwareTime()) {
        using namespace std::chrono;
        auto n = system_clock::now();
        const long long ticks = duration_cast<nanoseconds>(n.time_since_epoch()).count();
        m_device->setHardwareTime(ticks);
    }

    const std::vector<size_t> channels({0});
    m_tx_stream = m_device->setupStream(SOAPY_SDR_TX, "CF32", channels);
    m_rx_stream = m_device->setupStream(SOAPY_SDR_RX, "CF32", channels);
    */
}

Lime::~Lime()
{
    /*
    if (m_device != nullptr) {
        if (m_tx_stream != nullptr) {
            m_device->closeStream(m_tx_stream);
        }

        if (m_rx_stream != nullptr) {
            m_device->closeStream(m_rx_stream);
        }
        SoapySDR::Device::unmake(m_device);
    }*/
}

void Lime::tune(double lo_offset, double frequency)
{
   /* if (not m_device) throw runtime_error("Soapy device not set up");

    SoapySDR::Kwargs offset_arg;
    offset_arg["OFFSET"] = to_string(lo_offset);
    m_device->setFrequency(SOAPY_SDR_TX, 0, m_conf.frequency, offset_arg);
    */
}

double Lime::get_tx_freq(void) const
{
    if (not m_device) throw runtime_error("Lime device not set up");

    // TODO lo offset
    return 0;//m_device->getFrequency(SOAPY_SDR_TX, 0);
}

void Lime::set_txgain(double txgain)
{
    m_conf.txgain = txgain;
    if (not m_device) throw runtime_error("Lime device not set up");
    //m_device->setGain(SOAPY_SDR_TX, 0, m_conf.txgain);
}

double Lime::get_txgain(void) const
{
    if (not m_device) throw runtime_error("Lime device not set up");
    return 0;//m_device->getGain(SOAPY_SDR_TX, 0);
}

SDRDevice::RunStatistics Lime::get_run_statistics(void) const
{
    RunStatistics rs;
    rs.num_underruns = underflows;
    rs.num_overruns = overflows;
    rs.num_late_packets = late_packets;
    rs.num_frames_modulated = num_frames_modulated;
    return rs;
}


double Lime::get_real_secs(void) const
{
    /*if (m_device) {
        long long time_ns = m_device->getHardwareTime();
        return time_ns / 1e9;
    }
    else {
        return 0.0;
    }*/
    return 0.0;
}

void Lime::set_rxgain(double rxgain)
{
    /*m_device->setGain(SOAPY_SDR_RX, 0, m_conf.rxgain);
    m_conf.rxgain = m_device->getGain(SOAPY_SDR_RX, 0);
    */
}

double Lime::get_rxgain(void) const
{
    return 0.0;//m_device->getGain(SOAPY_SDR_RX, 0);
}

size_t Lime::receive_frame(
        complexf *buf,
        size_t num_samples,
        struct frame_timestamp& ts,
        double timeout_secs)
{
    /*int flags = 0;
    long long timeNs = ts.get_ns();
    const size_t numElems = num_samples;

    void *buffs[1];
    buffs[0] = buf;

    int ret = m_device->activateStream(m_rx_stream, flags, timeNs, numElems);
    if (ret != 0) {
        throw std::runtime_error(string("Soapy activate RX stream failed: ") +
                SoapySDR::errToStr(ret));
    }
    m_rx_stream_active = true;

    int n_read = m_device->readStream(
            m_rx_stream, buffs, num_samples, flags, timeNs);

    ret = m_device->deactivateStream(m_rx_stream);
    if (ret != 0) {
        throw std::runtime_error(string("Soapy deactivate RX stream failed: ") +
                SoapySDR::errToStr(ret));
    }
    m_rx_stream_active = false;

    if (n_read < 0) {
        throw std::runtime_error(string("Soapy failed to read from RX stream : ") +
                SoapySDR::errToStr(ret));
    }

    ts.set_ns(timeNs);
    
    return n_read;
    */
   return 0;
}


bool Lime::is_clk_source_ok() const
{
    // TODO
    return true;
}

const char* Lime::device_name(void) const
{
    return "Lime";
}

double Lime::get_temperature(void) const
{
    // TODO Unimplemented
    // LimeSDR exports 'lms7_temp'
    return std::numeric_limits<double>::quiet_NaN();
}

void Lime::transmit_frame(const struct FrameData& frame)
{
    if (not m_device) throw runtime_error("Lime device not set up");
/*
    long long int timeNs = frame.ts.get_ns();
    // muting and mutenotimestamp is handled by SDR
    const bool has_time_spec = (m_conf.enableSync and frame.ts.timestamp_valid);

    if (not m_tx_stream_active) {
        int flags = has_time_spec ? SOAPY_SDR_HAS_TIME : 0;
        int ret = m_device->activateStream(m_tx_stream, flags, timeNs);
        if (ret != 0) {
            throw std::runtime_error(string("Soapy activate TX stream failed: ") +
                    SoapySDR::errToStr(ret));
        }
        m_tx_stream_active = true;
    }

    // The frame buffer contains bytes representing FC32 samples
    const complexf *buf = reinterpret_cast<const complexf*>(frame.buf.data());
    const size_t numSamples = frame.buf.size() / sizeof(complexf);
    if ((frame.buf.size() % sizeof(complexf)) != 0) {
        throw std::runtime_error("Soapy: invalid buffer size");
    }

    // Stream MTU is in samples, not bytes.
    const size_t mtu = m_device->getStreamMTU(m_tx_stream);

    size_t num_acc_samps = 0;
    while (num_acc_samps < numSamples) {

        const void *buffs[1];
        buffs[0] = buf + num_acc_samps;

        const size_t samps_to_send = std::min(numSamples - num_acc_samps, mtu);

        const bool eob_because_muting = m_conf.muting;
        const bool end_of_burst = eob_because_muting or (
                frame.ts.timestamp_valid and
                frame.ts.timestamp_refresh and
                samps_to_send <= mtu );

        int flags = 0;

        auto num_sent = m_device->writeStream(
                m_tx_stream, buffs, samps_to_send, flags, timeNs);

        if (num_sent == SOAPY_SDR_TIMEOUT) {
            continue;
        }
        else if (num_sent == SOAPY_SDR_OVERFLOW) {
            overflows++;
            continue;
        }
        else if (num_sent == SOAPY_SDR_UNDERFLOW) {
            underflows++;
            continue;
        }

        if (num_sent < 0) {
            etiLog.level(error) << "Unexpected stream error " <<
                SoapySDR::errToStr(num_sent);
            throw std::runtime_error("Fault in Soapy");
        }

        timeNs += 1e9 * num_sent/m_conf.sampleRate;

        num_acc_samps += num_sent;

        if (end_of_burst) {
            int ret_deact = m_device->deactivateStream(m_tx_stream);
            if (ret_deact != 0) {
                throw std::runtime_error(
                        string("Soapy activate TX stream failed: ") +
                        SoapySDR::errToStr(ret_deact));
            }
            m_tx_stream_active = false;
        }

        if (eob_because_muting) {
            break;
        }
    }
    num_frames_modulated++;
    */
}

} // namespace Output

#endif // HAVE_LIMESDR

