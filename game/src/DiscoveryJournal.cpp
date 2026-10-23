#include "IslandSimulator/DiscoveryJournal.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <sstream>

namespace island {
namespace {
using enum JournalKind;
// The order the list shows them in. A save keeps the ids, so entries may be added or reordered
// freely; an id must never be reused for something else.
constexpr std::array Entries{
    JournalEntry{"hut","The resident's hut",Place,30.0f},
    JournalEntry{"jetty","Jetty",Place,25.0f},
    JournalEntry{"canoe","Dugout canoe",Place,15.0f},
    JournalEntry{"campfire","Campfire",Place,20.0f},
    JournalEntry{"lookout","Lookout tower",Place,30.0f},
    JournalEntry{"bridge","Rope bridge",Place,25.0f},
    JournalEntry{"helicopter","Helicopter",Place,35.0f},
    JournalEntry{"shipwreck","Shipwreck",Place,45.0f},
    JournalEntry{"freighter","Stranded coaster",Place,60.0f},
    JournalEntry{"lifeboat","Coaster's lifeboat",Place,25.0f},
    JournalEntry{"plane-wreck","Fighter plane wreck",Place,30.0f},
    JournalEntry{"lighthouse","Lighthouse",Place,70.0f},
    JournalEntry{"cottage","Keepers' cottage",Place,25.0f},
    JournalEntry{"whale-skeleton","Whale skeleton",Place,30.0f},
    JournalEntry{"idol","Stone idol",Place,30.0f},
    JournalEntry{"help-sign","HELP ME in stones",Place,25.0f},
    JournalEntry{"seabird-islet","Seabird islet",Place,80.0f},
    JournalEntry{"bottles","Washed-up bottles",Place,8.0f},
    JournalEntry{"bunker","Bunker",Place,25.0f},
    JournalEntry{"mine","Old mine",Place,25.0f},
    JournalEntry{"mine-chamber","Mine chamber",Place,8.0f},
    JournalEntry{"cave","Cave",Place,20.0f},
    JournalEntry{"volcano","Volcano",Place,90.0f},
    JournalEntry{"lava-flow","Lava flow",Place,30.0f},
    JournalEntry{"windthrow","Fallen jungle giant",Place,25.0f},
    JournalEntry{"foxfire","Foxfire",Place,8.0f},
    JournalEntry{"termite-nest","Termite nest",Place,8.0f},
    JournalEntry{"screw-pines","Screw pines",Place,15.0f},
    JournalEntry{"cascade","Cascade and plunge pool",Place,20.0f},
    JournalEntry{"wetland","Seasonal wetland",Place,25.0f},
    JournalEntry{"mangroves","Mangroves",Place,25.0f},
    JournalEntry{"goat-basin","Rain basin",Place,10.0f},
    JournalEntry{"tide-pools","Tide pools",Place,10.0f},
    JournalEntry{"turtle-nests","Turtle nests",Place,8.0f},
    JournalEntry{"passing-ship","Passing freighter",Place,1100.0f},
    JournalEntry{"resident","The resident",Creature,30.0f},
    JournalEntry{"bear","Bear",Creature,35.0f},
    JournalEntry{"deer","Deer",Creature,30.0f},
    JournalEntry{"boars","Wild boars",Creature,30.0f},
    JournalEntry{"goats","Feral goats",Creature,35.0f},
    JournalEntry{"monkeys","Monkeys",Creature,25.0f},
    JournalEntry{"crocodiles","Crocodiles",Creature,30.0f},
    JournalEntry{"coastal-birds","Coastal birds",Creature,30.0f},
    JournalEntry{"parrots","Parrots",Creature,30.0f},
    JournalEntry{"fruit-doves","Fruit doves",Creature,25.0f},
    JournalEntry{"seabirds","Seabirds",Creature,50.0f},
    JournalEntry{"frigatebirds","Frigatebirds",Creature,80.0f},
    JournalEntry{"boobies","Brown boobies",Creature,60.0f},
    JournalEntry{"reef-herons","Reef herons",Creature,30.0f},
    JournalEntry{"fruit-bats","Fruit bats",Creature,30.0f},
    JournalEntry{"mine-bats","Mine bats",Creature,20.0f},
    JournalEntry{"snakes","Ground snakes",Creature,8.0f},
    JournalEntry{"tree-snakes","Tree snakes",Creature,10.0f},
    JournalEntry{"geckos","Geckos",Creature,8.0f},
    JournalEntry{"skinks","Skinks",Creature,8.0f},
    JournalEntry{"frogs","Frogs",Creature,8.0f},
    JournalEntry{"mice","Mice",Creature,8.0f},
    JournalEntry{"rats","Rats",Creature,8.0f},
    JournalEntry{"crabs","Beach crabs",Creature,12.0f},
    JournalEntry{"hermit-crabs","Hermit crabs",Creature,8.0f},
    JournalEntry{"red-crabs","Red crab migration",Creature,15.0f},
    JournalEntry{"termite-flight","Termite nuptial flight",Creature,12.0f},
    JournalEntry{"coconut-crabs","Coconut crabs",Creature,10.0f},
    JournalEntry{"spiders","Spiders",Creature,6.0f},
    JournalEntry{"butterflies","Butterflies",Creature,12.0f},
    JournalEntry{"dragonflies","Dragonflies",Creature,12.0f},
    JournalEntry{"flies","Flies",Creature,5.0f},
    JournalEntry{"ants","Ants",Creature,4.0f},
    JournalEntry{"cicadas","Cicadas",Creature,8.0f},
    JournalEntry{"beetles","Beetles",Creature,5.0f},
    JournalEntry{"fireflies","Fireflies",Creature,20.0f},
    JournalEntry{"sea-turtles","Sea turtles",Creature,25.0f},
    JournalEntry{"hatchlings","Turtle hatchlings",Creature,12.0f},
    JournalEntry{"fish","Shoaling fish",Creature,15.0f},
    JournalEntry{"reef-fish","Reef fish",Creature,8.0f},
    JournalEntry{"stream-fish","Stream fish",Creature,8.0f},
    JournalEntry{"stingrays","Stingrays",Creature,15.0f},
    JournalEntry{"octopuses","Octopuses",Creature,8.0f},
    JournalEntry{"shark","Shark",Creature,40.0f},
    JournalEntry{"dolphins","Dolphins",Creature,60.0f},
    JournalEntry{"flying-fish","Flying fish",Creature,60.0f},
    JournalEntry{"whales","Humpback whales",Creature,500.0f},
};
constexpr std::string_view Header="island-simulator-journal";
}

DiscoveryJournal::DiscoveryJournal():found_(Entries.size(),false) {}

std::span<const JournalEntry> DiscoveryJournal::Catalogue() noexcept {return Entries;}

std::optional<std::size_t> DiscoveryJournal::IndexOf(const std::string_view id) noexcept {
    for(std::size_t i=0;i<Entries.size();++i) if(Entries[i].id==id) return i;
    return std::nullopt;
}

int DiscoveryJournal::Count(const JournalKind kind) noexcept {
    return static_cast<int>(std::count_if(Entries.begin(),Entries.end(),[kind](const JournalEntry& e) {return e.kind==kind;}));
}

bool DiscoveryJournal::Discover(const std::size_t index) {
    if(index>=found_.size()||found_[index]) return false;
    found_[index]=true;
    order_.push_back(index);
    return true;
}

int DiscoveryJournal::FoundCount(const JournalKind kind) const noexcept {
    return static_cast<int>(std::count_if(order_.begin(),order_.end(),[kind](const std::size_t i) {return Entries[i].kind==kind;}));
}

void DiscoveryJournal::Clear() {
    std::fill(found_.begin(),found_.end(),false);
    order_.clear();
}

std::string DiscoveryJournal::ToText() const {
    std::string text;
    text.append(Header).append(" ").append(std::to_string(CurrentSchema)).append("\n");
    for(const std::size_t index:order_) text.append("found ").append(Entries[index].id).append("\n");
    return text;
}

std::optional<DiscoveryJournal> DiscoveryJournal::Parse(const std::string_view text,std::string& error) {
    if(text.size()>MaximumSerializedBytes) {error="the journal record is larger than "+std::to_string(MaximumSerializedBytes)+" bytes";return std::nullopt;}
    DiscoveryJournal journal;
    std::size_t start=0;
    bool headed=false;
    while(start<text.size()) {
        std::size_t end=text.find('\n',start);
        if(end==std::string_view::npos) end=text.size();
        std::string_view line=text.substr(start,end-start);
        start=end+1;
        if(!line.empty()&&line.back()=='\r') line.remove_suffix(1);
        if(line.empty()) continue;
        if(!headed) {
            const std::string expected=std::string(Header)+" "+std::to_string(CurrentSchema);
            if(line!=expected) {error="the journal record does not start with '"+expected+"'";return std::nullopt;}
            headed=true;
            continue;
        }
        constexpr std::string_view Found="found ";
        if(!line.starts_with(Found)) {error="unexpected journal line '"+std::string(line)+"'";return std::nullopt;}
        if(const auto index=IndexOf(line.substr(Found.size()))) journal.Discover(*index);
    }
    if(!headed) {error="the journal record is empty";return std::nullopt;}
    error.clear();
    return journal;
}

bool JournalSighted(const std::array<float,3>& eye,const std::array<float,3>& forward,const std::array<float,3>& target,
                    const float radius,const float reach,const float halfAngleRadians) noexcept {
    const float dx=target[0]-eye[0],dy=target[1]-eye[1],dz=target[2]-eye[2];
    const float distance=std::sqrt(dx*dx+dy*dy+dz*dz);
    if(!std::isfinite(distance)||!(radius>=0.0f)||!(reach>=0.0f)) return false;
    if(distance<=radius) return true;
    if(distance-radius>reach) return false;
    const float along=(dx*forward[0]+dy*forward[1]+dz*forward[2])/distance;
    const float angle=std::acos(std::clamp(along,-1.0f,1.0f));
    return angle-std::asin(std::min(1.0f,radius/distance))<=halfAngleRadians;
}

}
