/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "oalaudiosource.hpp"
#include "oalcheckerror.hpp"
#include "oalaudiobuffer.hpp"

namespace xng {
    int convertType(AudioSource::SourceType type) {
        switch (type) {
            case AudioSource::UNDETERMINED:
                return AL_UNDETERMINED;
            case AudioSource::STATIC:
                return AL_STATIC;
            case AudioSource::STREAMING:
                return AL_STREAMING;
        }
        throw std::runtime_error("Unrecognized type");
    }

    OALAudioSource::OALAudioSource(ALuint sourceHandle) : handle(sourceHandle) {}

    OALAudioSource::~OALAudioSource() {
        alDeleteSources(1, &handle);
        checkOALError();
    }

    void OALAudioSource::play() {
        alSourcePlay(handle);
        checkOALError();
    }

    void OALAudioSource::pause() {
        alSourcePause(handle);
        checkOALError();
    }

    void OALAudioSource::stop() {
        alSourceStop(handle);
        checkOALError();
    }

    void OALAudioSource::rewind() {
        alSourceRewind(handle);
        checkOALError();
    }

    void OALAudioSource::setPitch(float pitch) {
        alSourcef(handle, AL_PITCH, pitch);
        checkOALError();
    }

    float OALAudioSource::getPitch() {
        float ret;
        alGetSourcef(handle, AL_PITCH, &ret);
        checkOALError();
        return ret;
    }

    void OALAudioSource::setGain(float gain) {
        alSourcef(handle, AL_GAIN, gain);
        checkOALError();
    }

    float OALAudioSource::getGain() {
        float ret;
        alGetSourcef(handle, AL_GAIN, &ret);
        checkOALError();
        return ret;
    }

    void OALAudioSource::setMaxDistance(float maxDistance) {
        alSourcef(handle, AL_MAX_DISTANCE, maxDistance);
        checkOALError();
    }

    float OALAudioSource::getMaxDistance() {
        float ret;
        alGetSourcef(handle, AL_MAX_DISTANCE, &ret);
        checkOALError();
        return ret;
    }


    void OALAudioSource::setRollOffFactor(float rollOffFactor) {
        alSourcef(handle, AL_ROLLOFF_FACTOR, rollOffFactor);
        checkOALError();
    }

    float OALAudioSource::getRollOffFactor() {
        float ret;
        alGetSourcef(handle, AL_ROLLOFF_FACTOR, &ret);
        checkOALError();
        return ret;
    }


    void OALAudioSource::setReferenceDistance(float referenceDistance) {
        alSourcef(handle, AL_REFERENCE_DISTANCE, referenceDistance);
        checkOALError();
    }

    float OALAudioSource::getReferenceDistance() {
        float ret;
        alGetSourcef(handle, AL_REFERENCE_DISTANCE, &ret);
        checkOALError();
        return ret;
    }

    void OALAudioSource::setMininumGain(float minGain) {
        alSourcef(handle, AL_MIN_GAIN, minGain);
        checkOALError();
    }

    float OALAudioSource::getMinimumGain() {
        float ret;
        alGetSourcef(handle, AL_MIN_GAIN, &ret);
        checkOALError();
        return ret;
    }


    void OALAudioSource::setMaximumGain(float maxGain) {
        alSourcef(handle, AL_MAX_GAIN, maxGain);
        checkOALError();
    }


    float OALAudioSource::getMaximumGain() {
        float ret;
        alGetSourcef(handle, AL_MAX_GAIN, &ret);
        checkOALError();
        return ret;
    }


    void OALAudioSource::setConeOuterGain(float coneOuterGain) {
        alSourcef(handle, AL_CONE_OUTER_GAIN, coneOuterGain);
        checkOALError();
    }

    float OALAudioSource::getConeOuterGain() {
        float ret;
        alGetSourcef(handle, AL_CONE_OUTER_GAIN, &ret);
        checkOALError();
        return ret;
    }

    void OALAudioSource::setConeInnerAngle(float innerAngle) {
        alSourcef(handle, AL_CONE_INNER_ANGLE, innerAngle);
        checkOALError();
    }

    float OALAudioSource::getConeInnerAngle() {
        float ret;
        alGetSourcef(handle, AL_CONE_INNER_ANGLE, &ret);
        checkOALError();
        return ret;
    }


    void OALAudioSource::setConeOuterAngle(float outerAngle) {
        alSourcef(handle, AL_CONE_OUTER_ANGLE, outerAngle);
        checkOALError();
    }


    float OALAudioSource::getConeOuterAngle() {
        float ret;
        alGetSourcef(handle, AL_CONE_OUTER_ANGLE, &ret);
        checkOALError();
        return ret;
    }

    void OALAudioSource::setPosition(Vec3f position) {
        alSource3f(handle, AL_POSITION, position.x, position.y, position.z);
        checkOALError();
    }

    Vec3f OALAudioSource::getPosition() {
        Vec3f ret;
        alGetSource3f(handle, AL_POSITION, &ret.x, &ret.y, &ret.z);
        checkOALError();
        return ret;
    }

    void OALAudioSource::setVelocity(Vec3f velocity) {
        alSource3f(handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
        checkOALError();
    }


    Vec3f OALAudioSource::getVelocity() {
        Vec3f ret;
        alGetSource3f(handle, AL_VELOCITY, &ret.x, &ret.y, &ret.z);
        checkOALError();
        return ret;
    }

    void OALAudioSource::setDirection(Vec3f direction) {
        alSource3f(handle, AL_DIRECTION, direction.x, direction.y, direction.z);
        checkOALError();
    }

    Vec3f OALAudioSource::getDirection() {
        Vec3f ret;
        alGetSource3f(handle, AL_DIRECTION, &ret.x, &ret.y, &ret.z);
        checkOALError();
        return ret;
    }


    void OALAudioSource::setSourceRelative(bool relative) {
        alSourcei(handle, AL_SOURCE_RELATIVE, relative);
        checkOALError();
    }

    bool OALAudioSource::getSourceRelative() {
        int ret;
        alGetSourcei(handle, AL_SOURCE_RELATIVE, &ret);
        checkOALError();
        return ret;
    }

    void OALAudioSource::setSourceType(AudioSource::SourceType type) {
        alSourcei(handle, AL_SOURCE_TYPE, convertType(type));
        checkOALError();
    }

    AudioSource::SourceType OALAudioSource::getSourceType() {
        int ret;
        alGetSourcei(handle, AL_SOURCE_TYPE, &ret);
        checkOALError();
        switch (ret) {
            case AL_UNDETERMINED:
                return UNDETERMINED;
            case AL_STATIC:
                return STATIC;
            case AL_STREAMING:
                return STREAMING;
            default:
                throw std::runtime_error("Source type not recognized");
        }
    }

    void OALAudioSource::setLooping(bool looping) {
        alSourcei(handle, AL_LOOPING, looping);
        checkOALError();
    }

    bool OALAudioSource::getLooping() {
        int ret;
        alGetSourcei(handle, AL_LOOPING, &ret);
        checkOALError();
        return ret;
    }

    AudioSource::SourceState OALAudioSource::getState() {
        ALint value = 0;
        alGetSourcei(handle, AL_SOURCE_STATE, &value);
        checkOALError();
        switch (value) {
            case AL_INITIAL:
                return INITIAL;
            case AL_PLAYING:
                return PLAYING;
            case AL_PAUSED:
                return PAUSED;
            case AL_STOPPED:
                return STOPPED;
            default:
                throw std::runtime_error("Unknown state value");
        }
    }

    void OALAudioSource::setBuffer(const AudioBuffer &buffer) {
        auto &b = dynamic_cast<const OALAudioBuffer &>(buffer);
        alSourcei(handle, AL_BUFFER, b.handle);
        checkOALError();
    }

    void OALAudioSource::clearBuffer() {
        alSourcei(handle, AL_BUFFER, 0);
        checkOALError();
    }

    void OALAudioSource::queueBuffers(std::vector<std::reference_wrapper<const AudioBuffer>> buffers) {
        //TODO: Queued buffers clear
        std::vector<ALuint> b(buffers.size());
        for (int i = 0; i < buffers.size(); i++) {
            auto &ob = dynamic_cast<const OALAudioBuffer &>(buffers[i].get());
            b[i] = ob.handle;
            bufferMapping.insert(std::pair<ALuint, std::reference_wrapper<const AudioBuffer>>(ob.handle, ob));
        }
        alSourceQueueBuffers(handle, buffers.size(), b.data());
        checkOALError();
    }

    std::vector<std::reference_wrapper<const AudioBuffer>> OALAudioSource::unqueueBuffers() {
        int available;
        alGetSourcei(handle, AL_BUFFERS_PROCESSED, &available);
        checkOALError();
        std::vector<ALuint> b(available);
        alSourceUnqueueBuffers(handle, available, b.data());
        checkOALError();
        std::vector<std::reference_wrapper<const AudioBuffer>> ret;
        for (int i = 0; i < available; i++) {
            ALuint v = b[i];
            ret.emplace_back(bufferMapping.at(v));
            bufferMapping.erase(v);
        }
        return ret;
    }

    ALuint OALAudioSource::getHandle() {
        return handle;
    }
}