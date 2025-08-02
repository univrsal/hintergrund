from itertools import combinations
from datetime import datetime
from typing import List, Dict, Any, Optional

from .manager import RuleManager
from ..database import DatabaseManager

def run(db: DatabaseManager, rule_manager: RuleManager) -> Optional[List[Dict[str, Any]]]:
    active_rules = rule_manager.get_active_rules()

    A = len(active_rules)
    
    # This will iterativly try combinations of rules, starting with the maximum number of rules
    # and going down to 1 rule, until it finds a combination that returns images.
    while A > 0:
        images_for_combination_level:List[List[Dict[str, Any]]] = []
        for combo in combinations(active_rules, A):
            # dict of all images that match the rules in the current combination
            images: Dict[int, Any] = {}
            shortest_cooldown = min(rule.cooldown for rule in combo) # in seconds

            for rule in combo:
                imgs = db.get_images(list(rule.tags), rule.require_all_tags)
                for img in imgs:
                    # parse last_used date to datetime
                    if img['id'] in images:
                        continue

                    if 'last_used' in img and img['last_used'] is not None:
                        last_used = img['last_used'] # format is 'YYYY-MM-DD HH:MM:SS'
                        last_used = datetime.strptime(last_used, '%Y-%m-%d %H:%M:%S')
                        if last_used is not None:
                            # check if cooldown is still active
                            if (datetime.now() - last_used).total_seconds() < shortest_cooldown:
                                continue
                    images[img['id']] = img

            if len(images) == 0:
                continue
            images_for_combination_level.append(list(images.values()))
            
        if len(images_for_combination_level) > 0:
            # find the longest list of images
            longest_images = max(images_for_combination_level, key=len)
            if len(longest_images) > 0:
                return longest_images
        A -= 1

    # if no images were found, just pick a random image
    random_image = db.pick_random_image()
    if random_image:
        return [random_image]