from typing import List

from pydantic import BaseModel


class InsertRequest(BaseModel):
    embedding: List[float]


class SearchRequest(BaseModel):
    query: List[float]
    k: int


class InsertResponse(BaseModel):
    status: str
    id: int


class SearchResult(BaseModel):
    id: int
    score: float


class SearchResponse(BaseModel):
    results: List[SearchResult]