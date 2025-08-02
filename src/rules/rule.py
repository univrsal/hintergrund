from pydantic import BaseModel
import random

class Rule(BaseModel):
    """Base class for rules in the system. If a rule applies, the image tags associated with the rule will be used to filter images."""

    name: str
    description: str
    tags: list[str] = []
    probability: float = 0.0
    enabled: bool = True
    cooldown: int = 0  # Cooldown in seconds, not used in this base class
    require_all_tags: bool = False  # If True, all tags must match; if False, any tag can match

    def is_active(self) -> bool:
        return self.enabled and random.random() < self.probability