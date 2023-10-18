/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include <limits>
#include <vector>
#include <cstring>
#include <string>

#include "xng/driver/sndfile/sndfileparser.hpp"

#include "xng/audio/audioformat.hpp"
#include "xng/asset/audio.hpp"


#include <sndfile.h>

namespace xng {
    struct LibSndBuffer {
        std::vector<char> data;
        size_t pos;
    };

    sf_count_t sf_vio_get_filelen(void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        return buffer->data.size();
    }

    sf_count_t sf_vio_seek(sf_count_t offset, int whence, void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        switch (whence) {
            case SF_SEEK_SET:
                buffer->pos = offset;
                break;
            case SF_SEEK_CUR:
                buffer->pos += offset;
                break;
            case SF_SEEK_END:
                buffer->pos = buffer->data.size() - offset;
                break;
            default:
                throw std::runtime_error("Invalid whence");
        }
        return buffer->pos;
    }

    sf_count_t sf_vio_read(void *ptr, sf_count_t count, void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        sf_count_t ret;
        for (ret = 0; ret < count && buffer->pos + ret < buffer->data.size(); ret++) {
            static_cast<char *>(ptr)[ret] = buffer->data.at(buffer->pos + ret);
        }
        buffer->pos += ret;
        return ret;
    }

    sf_count_t sf_vio_write(const void *ptr, sf_count_t count, void *user_data) {
        throw std::runtime_error("VIO Write");
    }

    sf_count_t sf_vio_tell(void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        return buffer->pos;
    }

    static Audio readAudio(const std::vector<char> &buf) {
        SF_VIRTUAL_IO virtio;
        virtio.get_filelen = &sf_vio_get_filelen;
        virtio.seek = &sf_vio_seek;
        virtio.read = &sf_vio_read;
        virtio.write = &sf_vio_write;
        virtio.tell = &sf_vio_tell;

        LibSndBuffer buffer{buf, 0};
        SF_INFO sfinfo;
        sfinfo.format = 0;
        SNDFILE *sndfile = sf_open_virtual(&virtio, SFM_READ, &sfinfo, &buffer);
        if (!sndfile) {
            throw std::runtime_error("Failed to open audio buffer");
        }

        if (sfinfo.frames < 1) {
            sf_close(sndfile);
            throw std::runtime_error("Bad sample count in audio buffer");
        }

        Audio ret;

        if (sfinfo.channels == 1) {
            ret.format = MONO16;
        } else if (sfinfo.channels == 2) {
            ret.format = STEREO16;
        } else if (sfinfo.channels == 3) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
                ret.format = BFORMAT2D_16;
            } else {
                sf_close(sndfile);
                throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
            }
        } else if (sfinfo.channels == 4) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
                ret.format = BFORMAT3D_16;
            } else {
                sf_close(sndfile);
                throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
            }
        } else {
            sf_close(sndfile);
            throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
        }

        ret.frequency = sfinfo.samplerate;

        std::vector<short> buff((sfinfo.frames * sfinfo.channels));

        sf_count_t num_frames = sf_readf_short(sndfile, buff.data(), sfinfo.frames);
        if (num_frames != sfinfo.frames) {
            sf_close(sndfile);
            throw std::runtime_error("Failed to read samples from audio data");
        }

        ret.buffer.resize(buff.size() * sizeof(short));

        for (auto i = 0; i < ret.buffer.size(); i++) {
            ret.buffer[i] = ((uint8_t *)buff.data())[i];
        }

        sf_close(sndfile);

        return ret;
    }

    ResourceBundle
    SndFileParser::read(const std::vector<char> &buffer, const std::string &hint, const std::string &path,
                        Archive *archive) const {
        ResourceBundle ret;
        ret.add("", std::make_unique<Audio>(readAudio(buffer)));
        return ret;
    }

    const std::set<std::string> &SndFileParser::getSupportedFormats() const {
        static const std::set<std::string> formats = {".wav"};
        return formats;
    }
}