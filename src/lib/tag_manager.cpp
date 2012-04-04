/*
 * Copyright (c) 2012, Prevas A/S
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Prevas A/S nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * DARC Tag Manager
 *
 * \author Morten Kjaergaard
 */

#include <darc/tag_manager.h>
#include <iostream>

namespace darc
{

void TagManager::remapTag(const std::string& original_tag, const std::string& remapped_tag)
{
  // Check what the remapped tag points to
  const std::string endpoint_tag = performRemappings(remapped_tag);

  // Make sure we have an existing ID for the remapped tag
  TranslatorListType::iterator remapped_entry = translator_list_.find(endpoint_tag);
  if(remapped_entry != translator_list_.end())
  {
    remapped_entry = translator_list_.insert(TranslatorListType::value_type(endpoint_tag, ID::create())).first;
  }

  // Create Remap Entry
  TagRemapListType::iterator remap_entry =
    tag_remap_list_.insert(TagRemapListType::value_type(original_tag, remapped_tag)).first;
  // todo: check for cycles

  // Check if we have an existing ID for the original tag
  TranslatorListType::iterator original_entry = translator_list_.find(original_tag);
  if(original_entry != translator_list_.end())
  {
    id_change_list_.insert(IDChangeListType::value_type(original_entry->second, remapped_entry->second));
    translator_list_.erase(original_entry);
  }

}

const std::string& TagManager::performRemappings(const std::string& tag)
{
  const std::string * current_tag = &tag;

  // Check for remappings
  TagRemapListType::iterator item = tag_remap_list_.find(*current_tag);
  while(item != tag_remap_list_.end())
  {
    current_tag = &(item->second);
    item = tag_remap_list_.find(*current_tag);
  }
  return *current_tag;
}

const TagID& TagManager::registerTag(const std::string& tag)
{
  const TagID& existing_id = lookup(tag);
  if(existing_id != ID::null())
  {
    return existing_id;
  }
  else
  {
    TranslatorListType::iterator item = translator_list_.insert(TranslatorListType::value_type(tag, ID::create())).first;
    return item->second;
  }
}

void TagManager::changeID(const TagID& original_id, const TagID& new_id)
{

}

const TagID& TagManager::lookup(const std::string& tag)
{
  const std::string& remapped_tag = performRemappings(tag);

  // Find Entry
  TranslatorListType::iterator id_item = translator_list_.find(remapped_tag);
  if(id_item != translator_list_.end())
  {
    return id_item->second;
  }
  else
  {
    return ID::null();
  }
}

const TagID& TagManager::verifyID(const TagID& id)
{
  IDChangeListType::iterator item = id_change_list_.find(id);
  if(item != id_change_list_.end())
  {
    return item->second;
  }
  else
  {
    return id;
  }
}

void TagManager::list()
{
  std::cout << "--------- TAGS ---------" << std::endl;
  for(TranslatorListType::iterator it = translator_list_.begin();
      it != translator_list_.end();
      it++)
  {
    std::cout << it->first << " -> " << it->second.short_string() << std::endl;
  }

  std::cout << "--------- REMAPPINGS ---------" << std::endl;
  for(TagRemapListType::iterator it = tag_remap_list_.begin();
      it != tag_remap_list_.end();
      it++)
  {
    std::cout << it->first << " -> " << it->second << std::endl;
  }

  std::cout << "--------- CHANGES ---------" << std::endl;
  for(IDChangeListType::iterator it = id_change_list_.begin();
      it != id_change_list_.end();
      it++)
  {
    std::cout << it->first << " -> " << it->second << std::endl;
  }
}

}
