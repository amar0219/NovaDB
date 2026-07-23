from fastapi import FastAPI, HTTPException

from .models import *
from .service import NovaDBService


app = FastAPI(
    title="NovaDB REST API",
    version="1.0.0"
)


@app.get("/")
def root():

    return {
        "message": "NovaDB API Running"
    }


@app.post("/insert", response_model=InsertResponse)
def insert(request: InsertRequest):

    try:
        return NovaDBService.insert(
            request.embedding
        )

    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=str(e)
        )


@app.post("/search", response_model=SearchResponse)
def search(request: SearchRequest):

    try:
        return NovaDBService.search(
            request.query,
            request.k
        )

    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=str(e)
        )


@app.get("/vector/{vector_id}")
def get_vector(vector_id: int):

    try:
        return NovaDBService.get(vector_id)

    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=str(e)
        )


@app.delete("/vector/{vector_id}")
def delete_vector(vector_id: int):

    try:
        return NovaDBService.delete(vector_id)

    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=str(e)
        )